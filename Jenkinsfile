@Library('kentik-build-tools') _
def kbt = new com.kentik.kbt.Kbt()
import java.text.SimpleDateFormat

node('worker-instance') {
    timestamps {
        try {
            stage('Build Setup') {
                env.WORKDIRS = "${WORKSPACE}/LinuxPBA ${WORKSPACE}/linux/CLI"
                env.VERSION = "Release_x86_64"
                def dateFormat = new SimpleDateFormat("yyyy-MM-dd")
                env.NAME = "${env.DISTRO}-${dateFormat}"

                deleteDir()
                checkout scm

                gitCommit()

                properties([
                    buildDiscarder(logRotator(artifactDaysToKeepStr: '365', artifactNumToKeepStr: '5', daysToKeepStr: '365', numToKeepStr: '5')),
                    disableConcurrentBuilds(),
                ])
            }

            stage('Jenkins Clean/Setup') {
                sh "sudo -E ${WORKSPACE}/setup.sh"
            }

            stage ('Build Binaries') {
                sh "for DIR in $WORKDIRS; do ( cd ${WORKSPACE}/$DIR ; make CONF=${VERSION} clean ; make CONF=${VERSION} ) 2>&1 | tee logfile.txt; done"
            }
            stage ('Build Debs') {
                sh "for DIR in $WORKDIRS; do cp ${WORKSPACE}/build/${VERSION}/GNU-linux DEBIAN/usr/sbin ; done; deb-pkg --build . ."

            }

            stage('Archive') {
                // Archive the output artifacts.
                archiveArtifacts artifacts: '*.deb'
                junit allowEmptyResults: true, testResults: 'test/results/*.xml'
            }

        } catch (e) {
            if (currentBuild.result != "ABORTED") {
                currentBuild.result = "FAILED"
            }
            throw e
        }
        finally {
            kbt.finishJob()
        }
    }
}
