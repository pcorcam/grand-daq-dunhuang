pipeline {
  agent {
    node {
      label 'cpp'
    }
  }


  stages {
    stage('Compile') {
      steps {
        echo 'Compile start...'
        sh 'cmake3 -DUNIT_TEST=ON -S . -Bbuild'
        sh 'cd build; cmake3 --build .'
        echo 'Compile done!'
      }
    }

    stage('Test') {
      environment {
        GRAND_DAQ_HOME="${env.WORKSPACE}"
        GRAND_CSDAQ_CONFIG="${GRAND_DAQ_HOME}/cfgs/csdaq"
        GRAND_CSDAQ_CONFIG_LOG="${GRAND_CSDAQ_CONFIG}/logging"
        GRAND_DUDAQ_CONFIG="${GRAND_DAQ_HOME}/cfgs/dudaq"
        GRAND_DUDAQ_CONFIG_LOG="${GRAND_DUDAQ_CONFIG}/logging"
      }

      steps {
        sh 'cd build; ctest3 -T test --no-compress-output'
      }
    }

  }

  post {
    always {
      // Archive the CTest xml output
      archiveArtifacts (
        artifacts: 'build/Testing/**/*.xml',
        fingerprint: true
      )

      // Process the CTest xml output with the xUnit plugin
      xunit (
        testTimeMargin: '3000',
        thresholdMode: 1,
        thresholds: [
          skipped(failureThreshold: '0'),
          failed(failureThreshold: '0')
        ],
        tools: [CTest(
          pattern: 'build/Testing/**/*.xml',
          deleteOutputFiles: true,
          failIfNotNew: false,
          skipNoTestFiles: true,
          stopProcessingIfError: true
        )]
      )

      // Clear the source and build dirs before next run
      deleteDir()
    }
  }
}  
