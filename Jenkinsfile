#!groovy

pipeline {
    agent any
    stages {
        stage('build') {
            steps {
                sh 'qmake -makefile'
                sh 'make clean'
                sh 'make'
            }
        }
        stage('package') {
            steps {
              sh 'rm -fr package_build ; mkdir package_build'
              sh 'cp myBrewApp/myBrewApp package_build'
              sh 'cp myBrewLib/libmyBrewLib* package_build'
              sh 'cp myBrewTests/myBrewTests package_build'
              sh 'cp images/* package_build'
              sh 'cp audio/* package_build'
              sh 'cd package_build'
              sh 'cd package_build; tar cvzf ../myBrew_v${BUILD_NUMBER}.tar.gz *'
            }
        }
        stage('deliver') {
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
        stage('static-analysis') {
          steps {
            sh 'cppcheck cppcheck --enable=all --inconclusive --xml --xml-version=2 . 2> cppcheck.xml'
            publishCppcheck pattern: 'cppcheck.xml'
          }
        }
        stage('unit-tests') {
          steps {
            sh 'rm -rf /DB; mkdir /DB'
            sh 'cd package_build; export LD_LIBRARY_PATH=. ; xvfb-run ./myBrewTests -o ../test_output.xml,xml'
            sh 'rm -rf package_build'
          }
        }
    }
    post {
      always{
        xunit (
            thresholds: [ skipped(failureThreshold: '0'), failed(failureThreshold: '0') ],
            tools: [ QTestLib(pattern: 'test_output.xml') ])
      }
    }
}
