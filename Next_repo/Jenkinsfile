/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 * ========================== NEXT Project ==================================
 */
 /**
 * @file     Jenkinsfile
 * @brief    File of the pipeline to build and test the repository of NEXT
 */

// groovylint-disable-next-line UnusedVariable
@Library('bricks_pipeline_lib') _
artifactoryInstance = Artifactory.server 'eu-https'


// set defaults, e.g. stopping previous builds
cipUtilities.setJobProperties()

void setJobProperties() {
  properties([
    // cron job
    pipelineTriggers([cron(env.BRANCH_NAME == 'main' ? '0 2 * * *' : '')]),
    // parameters to customize the containers and test framework versions
    parameters([
      // Enabling Integrationtests on pipeline run
      booleanParam(name: 'integrationtest',
                    defaultValue: false,
                    description: 'Enable Integrationtest'),
      // Test level of the integration test (tags)
      string(name: 'integration_test_filter',
                defaultValue: 'default',
                description: 'Used tags: none, nightly, databridge, controlbridge, release'),
      // Flag to enable email report
      booleanParam(name: 'report',
                defaultValue: false,
                description: 'Enable if reports should be send (nightly build always enabled)'),
      // nightly build email addresses, default Janker Fabian (uif41320),
      // Fritz Jannik (uidr9189), Achim Groner (uie80129)
      string(name: 'reporter_emails',
             defaultValue: '''fabian.janker@continental-corporation.com;
jannik.fritz@continental-corporation.com;
achim.groner@continental-corporation.com''',
             description: 'E-Mails for testresults of the integration tests')
    ])
  ])
}

void setupIntegrationTest() {
  checkout scm

  // Unstash your previous builds, you will find those names in postbuild reporting stage since it's doing same
  unstash 'next.generic.vs2017.build'
  unstash 'next.generic.vs2017.test.GENERIC.default'
}

void runIntegrationTest() {
  try {
    // Run CONTEST
    echo 'Start ConTest'
    setup_file = "${env.WORKSPACE}\\tests\\integration\\setup.pytest"
    test_config = "${env.WORKSPACE}\\tests\\cfg\\Integration_Test_Jenkins_config.ini"
    // expression to check if the integration test has an tag if none the enabled test of the ini file is used
    bat """
      @echo off
      setlocal
      call %PYTHONPATH% -u C:\\software\\tools\\contest\\main.py ^
        -c  ${test_config} ^
        -l ${env.WORKSPACE} ^
        --setup-file ${setup_file} ^
        -r auto ^
        --random ^
        --report-dir ${env.WORKSPACE}/conan_workarea/contest_reports
      endlocal
    """
  } catch (e) {
    // if the integration test has a failed test, it is exiting with code 2 and the test report is not builded
    // setting buildReslult to SUCCESS it is generating the test result
    if (e.getMessage() != null) {
      if (e.getMessage().contains('script returned exit code 2') ||
          e.getMessage().contains('script returned exit code 1')) {
        catchError(buildResult: 'SUCCESS', stageResult: 'UNSTABLE', catchInterruptions: true) {
          error e.getMessage()
        }
      } else {
        echo e.getMessage()
        throw e
          }
    } else {
      echo 'ConTest crashed without error message'
      throw e
    }
  } finally {
    junit 'conan_workarea/contest_reports/reports_*/contest__txt_reports/TEST_RESULT.xml'
    sendEmail()
  }
}

void runNextPyApiUnitTest(isWindows) {
  try {
    checkout scm
    if (isWindows) {
      // On windows
      // Run next_py_api Unit test
      echo 'Start next_py_api Unit Test'
      requirements_windows_file = "${env.WORKSPACE}\\tests\\unit\\py_api\\requirements.txt"
      unit_test_windows_dir = "${env.WORKSPACE}\\tests\\unit\\py_api"

      // install virtual python environment
      echo 'install python library from requirements'
      bat "python -m pip install -r ${requirements_windows_file}"
      echo 'run pytest'
      bat "python -m pytest ${unit_test_windows_dir} -v"
    } else {
      // On ubuntu
      // Run next_py_api Unit test
      echo 'Start next_py_api Unit Test'
      requirements_ubuntu_file = "${env.WORKSPACE}/tests/unit/py_api/requirements.txt"
      unit_test_ubuntu_dir = "${env.WORKSPACE}/tests/unit/py_api"

      // install virtual python environment
      echo 'install python library from requirements'
      sh "python3 -m pip install -r ${requirements_ubuntu_file}"
      echo 'run pytest'
      sh "python3 -m pytest ${unit_test_ubuntu_dir} -v"
    }

  } catch (err) {
    warning("error to run next_py_api Unit Test: ${err}")
    sendEmail()
  }

}

void sendEmail() {
  echo "Result: ${currentBuild.currentResult}"

  // enabling or disabling report with the parameter section in Jenkins
  if (params.report || env.BRANCH_NAME == 'main') {
    // switch case to detect the build result: SUCCESS, UNSTABLE, FAILED (default of the switch case)
    switch (currentBuild.currentResult) {
      case 'SUCCESS':
        break
      case 'UNSTABLE':
        emailext(
            mimeType: 'text/html',
            // Disabling lint for next line because lint treat warning as error allthough right syntax
            /* groovylint-disable-next-line GStringExpressionWithinString */
            body: '''${SCRIPT, template="groovy-html.template"}''',
            subject: "Job '${env.JOB_NAME}' (${env.BUILD_NUMBER}) is unstable",
            to: "${params.reporter_emails}"
          )
        break
      default:
        emailext(
          mimeType: 'text/html',
          // Disabling lint for next line because lint treat warning as error allthough right syntax
          /* groovylint-disable-next-line GStringExpressionWithinString */
          body: '''${SCRIPT, template="groovy-html.template"}''',
          subject: "Job '${env.JOB_NAME}' (${env.BUILD_NUMBER}) has failed",
          to: "${params.reporter_emails}"
        )
        break
    }
  }
}

void wheel_uploader() {
    checkout scm

    unstash 'next.generic.vs2017.build'
    withCredentials(
    [usernamePassword(credentialsId: 'artifactory_identity_token',
    usernameVariable: 'PYPI_USER',
    passwordVariable: 'PYPI_TOKEN')]) {
    bat """
    (echo [distutils]& \
    echo.index-servers = artifactory& \
    echo.[artifactory]& \
    echo.repository: https://eu.artifactory.conti.de/artifactory/api/pypi/c_adas_cip_pypi_l& \
    echo.username: ${PYPI_USER}& \
    echo.password: ${PYPI_TOKEN}& echo.) > C:\\Users\\%username%\\.pypirc
    """
    withCredentials(
    [usernamePassword(credentialsId: 'uic91112',
     passwordVariable: 'password',
     usernameVariable: 'USER')])
    {
        def downloadSpec = """
        {
        "files": [
            {
            "pattern": "c_adas_cip_toolchain_generic_v/Tools_external/Development_Framework/python/Windows/3.9.12.zip",
            "target": "${WORKSPACE}/../python/python-3.9.12/",
            "explode": "true",
            "flat": "true"
            }
        ]
        }
        """
        echo "Downloading with spec: $downloadSpec"
        artifactoryInstance.download spec: downloadSpec
        echo "Downloading is finished"
        echo "Creating python virtual environment"
        powershell """
            ${WORKSPACE}\\..\\python\\python-3.9.12\\3.9.12\\python.exe -m venv venv
        """
        }
        echo "Python virtual environment is created"
        echo "Activate python environment"
        powershell """
            .\\venv\\Scripts\\Activate.ps1
        """
        echo "Install python library"
        powershell """
            python -m pip install -r ${WORKSPACE}\\src\\py_api\\install\\reqs_for_wheel_build_and_upload.txt
        """
        echo "Upload the wheel packages"
        powershell """
            python ${WORKSPACE}\\src\\py_api\\install\\wheel_release.py
        """
        }
}

  /**
  checkChanges is a function which scans the changed files and
  sets the build level depending on these files.

  Build Levels:
    1: only changes in the doc folder -> build only doc
    2: only changes in the next_api_py folders (src/py_api, tests/unit/py_api) -> build only next_api_py Unit Test
    3: only changes in the test folder -> build all, execute Unit Test, ignore Integrationtest
    4: only changes in the src folder -> build all, execute Unit Test, start Integrationtest
    5: changes in any other folder -> build all, execute Unit Test, start Integrationtest
  */
Map checkChanges() {
  checkout scm
  def buildLevelMap = [:]
  def buildLevel = 0
  def shouldRunNextPyApiUnitTest = false
  def shouldUploadNewWheelPackageDistribution = false
  def changeList = sh(script: "git diff-tree -r --no-commit-id --name-only HEAD origin/main",
                      returnStdout: true).trim()
  def changedFiles = changeList.split('\n')
  echo "Changed Files: ${changedFiles}"

  try {
    for(int i = 0; i < changedFiles.size(); i++) {
      if (!changedFiles[i].contains('diff-tree')) {
        if (changedFiles[i].contains('doc') && buildLevel != 2
            && buildLevel != 3 && buildLevel != 4 && buildLevel != 5) {
          buildLevel = 1
        } else if (changedFiles[i].contains('src/py_api') || changedFiles[i].contains('tests/unit/py_api')) {
          shouldRunNextPyApiUnitTest = true
          shouldUploadNewWheelPackageDistribution = true
          if (buildLevel != 3 && buildLevel != 4 && buildLevel != 5) {
            buildLevel = 2
          }
          if (changedFiles[i].contains('src/py_api/install')) {
            shouldUploadNewWheelPackageDistribution = true
          }
        } else if (changedFiles[i].contains('test') && buildLevel != 4 && buildLevel != 5){
          if (changedFiles[i].contains('tests/integration') || changedFiles[i].contains('tests/cfg')) {
            buildLevel = 5
          } else {
            buildLevel = 3
          }
        } else if (changedFiles[i].contains('src') && buildLevel != 5){
          buildLevel = 4
        } else {
          buildLevel = 5
        }
      }
    }
  } catch (err) {
    warning("changed files couldn't be analyzed. Set build Level to 5: ${err}")
    buildLevel = 5
  }

  if (buildLevel == 0) {
    buildLevel = 5
  }

  echo "Build Level: ${buildLevel}"
  if (shouldRunNextPyApiUnitTest) {
    echo "The next_py unit test will run"
  }
  echo "shouldUploadNewWheelPackageDistribution: ${shouldUploadNewWheelPackageDistribution}"
  if (shouldUploadNewWheelPackageDistribution) {
    echo "The next_py wheel package should be uploaded"
  }
  buildLevelMap["buildLevel"] = buildLevel
  buildLevelMap["shouldRunNextPyApiUnitTest"] = shouldRunNextPyApiUnitTest
  buildLevelMap["shouldUploadNewWheelPackageDistribution"] = shouldUploadNewWheelPackageDistribution

  return buildLevelMap
}

void removeTasksToBuildOnlyDocumentation() {
  removeTask(match: [variant: "generic", platform: "vs2017", target: "build"])
  removeTask(match: [variant: "generic", platform: "vs2017_debug", target: "build"])
  removeTask(match: [variant: "generic", platform: "linux64_x86_gcc_7-v2-release", target: "build"])
  removeTask(match: [variant: "generic", platform: "linux64_x86_gcc_7-v2", target: "build"])
  removeTask(match: [variant: "generic", platform: "ubuntu_20_04_x86_64-release", target: "build"])
  removeTask(match: [variant: "generic", platform: "ubuntu_20_04_x86_64", target: "build"])

  removeTask(match: [variant: "generic", platform: "vs2017", target: "test"])
  removeTask(match: [variant: "generic", platform: "ubuntu_20_04_x86_64-release", target: "test"])

  removeTask(match: [variant: "normal_debug", platform: "vs2017_debug", target: "build"])
  removeTask(match: [variant: "normal_debug", platform: "linux64_x86_gcc_7-v2", target: "build"])
  removeTask(match: [variant: "normal_debug", platform: "ubuntu_20_04_x86_64", target: "build"])
  removeTask(match: [variant: "normal_debug", platform: "vs2017_debug", target: "test"])
  removeTask(match: [variant: "normal_debug", platform: "ubuntu_20_04_x86_64", target: "test"])

  removeTask(match: [variant: "delivery", platform: "vs2017", target: "build"])
  removeTask(match: [variant: "delivery", platform: "linux64_x86_gcc_7-v2-release", target: "build"])
  removeTask(match: [variant: "delivery", platform: "ubuntu_20_04_x86_64-release", target: "build"])

}

void main() {
withCredentials(bindings: [usernamePassword(
    credentialsId: 'doc_service',
    usernameVariable: 'HTD_CREDS_USR',
    passwordVariable: 'HTD_CREDS_PSW')])
{
  setJobProperties()

  bricks([memoryRequest: '48Gi', cpuRequest: '8.0'], {
    bricksManager.generateTasks()  // generates the bricks tasks as defined in the conf/build.yaml
    bricksManager.generateToolTasks()  // generates tool tasks from conf/build.yml
    bricksManager.applyGeneratedTasks()  // applies all generated tasks to internal pipeline structure.

    buildLevelMap = checkChanges()
    buildLevel = buildLevelMap["buildLevel"]
    shouldRunNextPyApiUnitTest = buildLevelMap["shouldRunNextPyApiUnitTest"]
    shouldUploadNewWheelPackageDistribution = buildLevelMap["shouldUploadNewWheelPackageDistribution"]

    //buildLevel 1: only changes in the doc folder -> build only doc
    if(buildLevel == 1) {
      removeTasksToBuildOnlyDocumentation()
    }

    try{
      isDraft = pullRequest?.draft
    } catch (MissingPropertyException e) {
      isDraft = false
    }

    // buildLevel 2: only changes in the next_api_py folders (src/py_api, tests/unit/py_api)
    // -> build only next_api_py Unit Test if PR is draft
    // Run the next_api_py unit test
    if (buildLevel == 2 && isDraft) {
      removeTask(match: [variant: "generic", platform: "vs2017", target: "doc"])
      removeTask(match: [variant: "delivery", platform: "vs2017", target: "doc"])
      removeTasksToBuildOnlyDocumentation()

    }
    if (shouldRunNextPyApiUnitTest){
      addTask(name: 'NextPyApiUnitTestWindows', platform: "vs2017", placement: 'build'){
        runNextPyApiUnitTest(1)
      }
      addTask(name: 'NextPyApiUnitTestUbuntu', platform: "ubuntu_20_04_x86_64", placement: 'build'){
        runNextPyApiUnitTest(0)
      }
    }
    addPhase(name: 'Integration_Test', priority: 350)
    // Integrationtest only if changes are not only in the doc or test folder
    // Mark as Failed if changes only in the next_api_py folders and is draft (buildLevel=2)
    if ((!isDraft && buildLevel > 3) || params.integrationtest || (!isDraft && buildLevel == 2)) {
      addTask(name: 'Integrationtest', label: "'uuas061a'", placement: 'Integration_Test') {
        echo "Hostname: ${NODE_NAME}"
        cleanWs()

        setupIntegrationTest()

        runIntegrationTest()
      }
    } else if ((buildLevel > 3) || (buildLevel == 2)) {
      addTask(name: 'Integrationtest', label: "windows", placement: 'Integration_Test') {
        error("Current Pull Request is a draft, skipping the integration test")
      }
    }

    if (shouldUploadNewWheelPackageDistribution) {
      addPhase(name: 'Wheel_Package_Distribution', priority: 355)
      addTask(name: 'WheelPackageDistribution', label: "windows", placement: 'Wheel_Package_Distribution') {
      echo "ENV BRANCH NAME: ${env.BRANCH_NAME}"
      echo "BRANCH NAME: ${BRANCH_NAME}"
      if (env.BRANCH_NAME == 'main') {
        echo "Call wheel_uploader function"
        wheel_uploader()
      }
      }
    }
  })
}
}

main()
