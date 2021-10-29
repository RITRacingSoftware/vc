pipeline {
    agent { 
        docker { 
            image 'vc'
            args '-u root'
        } 
    }
    stages {
        stage('build') {
            steps {
                sh './build.sh'
            }
        }
    }
    post { 
        always { 
            cleanWs()
        }
    }
}