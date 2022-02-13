pipeline {
    agent { 
        docker { 
            image 'vc'
            args '-u root:root'
        } 
    }
    stages {
        stage('build') {
            steps {
                sh './build.sh'
            }
        }
    }
}