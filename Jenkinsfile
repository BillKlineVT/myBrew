#!groovy

String cron_string = ""

if (BRANCH_NAME == "master")
{
  cron_string = "@nightly"
}
else if (BRANCH_NAME.startsWith("release/"))
{
  cron_string = "@weekly"
}
else if (BRANCH_NAME == "develop")
{
  cron_string = "@hourly"
}
else if  (BRANCH_NAME.startsWith("feature/"))
{
  cron_string = "@weekly"
}

pipeline {
  environment {
    registry = "billklinefelter/mybrew_x86_64"
    registryCredential = 'dockerhub'
    dockerImage = ''
  }
  agent any
  /*{
    docker {
      image 'billklinefelter/jenkins-qt5'
      args '-p 3000:3000'
    }
  }*/
  triggers {
    cron(cron_string)
  }
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
          sh 'cp -a myBrewTests/GUITest/GUITest package_build'
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
          def server = Artifactory.newServer url: 'http://artifactory:8081/artifactory', credentialsId: 'jenkins-artifactory-key'
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
          withCredentials([usernameColonPassword(credentialsId: 'jenkins-artifactory-key', variable: 'LOGIN')]) {
            sh 'ssh -t root@192.168.1.5 "curl -u $LOGIN -X GET "http://artifactory:8081/artifactory/generic-local/myBrew/myBrew_v\$BUILD_NUMBER.tar.gz" --output myBrew_v\$BUILD_NUMBER.tar.gz"'
            sh 'ssh -t root@192.168.1.5 "cd; rm -f SUT; mkdir -p SUT; cd SUT; tar xzvf ../myBrew_v\$BUILD_NUMBER.tar.gz; export DISPLAY=:1; cd GUITest; mvn clean package; java -jar target/GUITest.myBrewGUITest-0.0.1-SNAPSHOT.jar > ../myBrew_v\${BUILD_NUMBER}_auto_gui_test_results.txt"'
            // upload test results to artifactory here...
          }
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
