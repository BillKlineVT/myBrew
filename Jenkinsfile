#!groovy

pipeline {
  environment {
    registry = "billklinefelter/mybrew_x86_64"
    registryCredential = 'dockerhub'
    dockerImage = ''
  }
  agent any
  stages {
    stage('build') {
        steps {
            sh 'qmake -makefile'
            sh 'make clean'
            sh 'make'
        }
    }
    stage('build-sikulix-auto-gui-test') {
      steps { 
          sh '''
            cd myBrewTests/GUITest/GUITest
            mvn clean package 
          '''
        }
    }
    stage('package') {
        steps {
          sh 'rm -fr package_build ; mkdir package_build'
          sh 'cp myBrewApp/myBrewApp package_build'
          sh 'mkdir -p package_build/lib; cp myBrewLib/libmyBrewLib* package_build/lib'
          sh 'cp myBrewTests/myBrewTests package_build'
          sh 'cp myBrewTests/GUITest/GUITest/target/GUITest.myBrewGUITest-0.0.1-SNAPSHOT.jar package_build'
          sh 'cp -a myBrewTests/GUITest/GUITest/target/GUITest.myBrewGUITest-0.0.1-SNAPSHOT.lib package_build'
          sh 'cp -a images package_build'
          sh 'cp -a audio package_build'
          sh 'cp -a python package_build'
          sh 'cd package_build'
          sh 'cd package_build; tar cvzf ../myBrew_v${BUILD_NUMBER}.tar.gz *'
        }
    }
    stage('static-analysis') {
      steps {
        sh 'cppcheck cppcheck --enable=all --inconclusive --xml --xml-version=2 . 2> cppcheck.xml'
        publishCppcheck pattern: 'cppcheck.xml'
      }
    }
    stage('unit-tests') {
      steps {
        sh 'rm -rf /DB; mkdir /DB'
        sh 'cd package_build; export LD_LIBRARY_PATH=./lib ; xvfb-run ./myBrewTests -o ../test_output.xml,xml'
        sh 'rm -rf package_build'
      }
    }
    stage('deliver-binary') {
      steps {
        script {
          def server = Artifactory.newServer url: 'http://artifactory:8081/artifactory', credentialsId: '64b21b56-0d9a-49d6-9ea1-399a1377b13f'
          def uploadSpec = """{
            "files": [
              {
                "pattern": "myBrew_v${BUILD_NUMBER}.tar.gz",
                "target": "generic-local/myBrew/"
              }
            ]
          }"""
          server.upload spec: uploadSpec, failNoOp: true
        }
      }
    }
    stage('build-docker-runtime-x86_64') {
      steps {
        script {
          dockerImage = docker.build(registry + ":$BUILD_NUMBER", "-f ./targets/x86_64/Dockerfile .")
        }
      }
    }
    stage('push-docker-runtime-x86_64') {
      steps { 
        script {
          docker.withRegistry( '', registryCredential ) {
            dockerImage.push()
            dockerImage.push("latest")
          }
        }
      }
    }
    stage('execute-sikulix-auto-gui-test') {
      steps { 
          script {
            def remote = [:]
            remote.name = "SystemUnderTest"
            remote.host = "192.168.1.5"
            remote.allowAnyHosts = true
          }
          withCredentials(([usernameColonPassword(credentialsId: 'c603ab1f-f27a-4da2-bea2-f010ee12676a', variable: 'USERPASS')]) {
            sh 'ssh -p $USERPASS -t root@192.168.1.5 "curl -u jenkins:AKCp5e2g4tWoK7tcbXF5qG946eiykvyJAsDkPwiQsZNC7upBmfdpeS2mjPF4uJC6YiQx5FrU6 -X GET "http://192.168.1.5:8081/artifactory/generic-local/myBrew/myBrew_v\$BUILD_NUMBER.tar.gz" --output myBrew_v\$BUILD_NUMBER.tar.gz"'
            sh 'ssh -p $USERPASS -t root@192.168.1.5 "cd; mkdir -p SUT; tar xzvf myBrew_v\$BUILD_NUMBER.tar.gz;cd ~/SUT/myBrewTests/GUITest/GUITest; export DISPLAY=:1; java -jar target/GUITest.myBrewGUITest-0.0.1-SNAPSHOT.jar > myBrew_auto_gui_test_results.txt"'
          }
          //sh '''
          //  cd myBrewTests/GUITest/GUITest
          //  export DISPLAY=:1
          //  java -jar target/GUITest.myBrewGUITest-0.0.1-SNAPSHOT.jar > myBrew_v\${BUILD_NUMBER}_auto_gui_test_results.txt
          //'''
          /*script {
          def server = Artifactory.newServer url: 'http://artifactory:8081/artifactory', credentialsId: '64b21b56-0d9a-49d6-9ea1-399a1377b13f'
          def uploadSpec = """{
            "files": [
              {
                "pattern": "myBrew_v${BUILD_NUMBER}_auto_gui_test_results.txt",
                "target": "generic-local/myBrew_testResults/"
              }
            ]
          }"""
          server.upload spec: uploadSpec, failNoOp: true
          }*/
      }
    }
  }
  post {
    always{
      xunit (
          thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ],
          tools: [ QtTest(pattern: 'test_output.xml') ])
    }
  }
}
