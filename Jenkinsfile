pipeline {
    agent { 
        docker { 
            image 'vc'
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