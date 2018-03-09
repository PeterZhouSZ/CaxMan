#!/bin/sh

# Check existence of required environment variable
if [ -z "$MAX_LOG_FILES" ]; then
	echo "MAX_LOG_FILES not set, setting to 10"
	export MAX_LOG_FILES=10
fi

: "${CONTEXT_ROOT:?Error - CONTEXT_ROOT unset or empty}"

# Set glassfish admin password to the one defined in ADMIN_PASSWORD or to a randomly
# generated one if ADMIN_PASSWORD doesn't exist
if [[ -z $ADMIN_PASSWORD ]]; then
	ADMIN_PASSWORD=$(date| md5sum | fold -w 8 | head -n 1)
	echo "##########GENERATED ADMIN PASSWORD: $ADMIN_PASSWORD  ##########"
fi
echo "AS_ADMIN_PASSWORD=" > /tmp/glassfishpwd
echo "AS_ADMIN_NEWPASSWORD=${ADMIN_PASSWORD}" >> /tmp/glassfishpwd
asadmin -u admin -W /tmp/glassfishpwd change-admin-password --domain_name domain1
asadmin start-domain
echo "AS_ADMIN_PASSWORD=${ADMIN_PASSWORD}" > /tmp/glassfishpwd
asadmin -u admin -W /tmp/glassfishpwd enable-secure-admin

# Set up glassfish
asadmin -u admin -W /tmp/glassfishpwd set server.network-config.protocols.protocol.http-listener-1.http.scheme-mapping=X-Forwarded-Proto
asadmin -u admin -W /tmp/glassfishpwd set server.admin-service.das-config.autodeploy-enabled=false
asadmin -u admin -W /tmp/glassfishpwd set-log-attributes com.sun.enterprise.server.logging.GFFileHandler.maxHistoryFiles=${MAX_LOG_FILES}

# Deploy with a custom context root and stop the domain
asadmin -u admin -W /tmp/glassfishpwd deploy --contextroot ${CONTEXT_ROOT} /app.war
asadmin -u admin -W /tmp/glassfishpwd stop-domain
rm /tmp/glassfishpwd

exec "$@"
