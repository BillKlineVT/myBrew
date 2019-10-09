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
              sh 'mkdir package_build'
              sh 'cp myBrew package_build'
              sh 'cp *.png package_build'
              sh 'cp *.jpg package_build'
              sh 'cp *.jpeg package_build'
              sh 'cp *.wav package_build'
              sh 'cd package_build'
              sh 'cd package_build; tar cvzf ../myBrew_v${BUILD_NUMBER}.tar.gz *'
              sh 'rm -rf package_build'
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
            sh 'cppcheck --xml --xml-version=2 . 2> cppcheck.xml'
            publishCppcheck pattern: 'cppcheck.xml'
          }
        }
    }
}
