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
        stage('deliver') {
            steps {
              sh 'mkdir package_build'
              sh 'cp myBrew package_build'
              sh 'cp *.png package_build'
              sh 'cp *.jpg package_build'
              sh 'cp *.jpeg package_build'
              sh 'cp *.wav package_build'
              sh 'cd package_build'
              sh 'tar cvzf ../myBrew_v${BUILD_NUMBER}.tar.gz *'
              sh 'cd ..'
              sh 'rm -rf package_build'
            }
        }
    }
}
