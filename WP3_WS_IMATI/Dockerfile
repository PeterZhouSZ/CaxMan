# Base this Docker image on one with a pre-configured glassfish server
FROM oracle/glassfish:5.0

# Copy entrypoint script and compiled source package
COPY docker-entrypoint.sh /entrypoint.sh
COPY target/WP3_WS_IMATI-1.0.war /app.war

# We want to have port 8080 visible from outside
EXPOSE 8080

ENTRYPOINT ["/entrypoint.sh"]

CMD ["asadmin", "start-domain", "-v"]
