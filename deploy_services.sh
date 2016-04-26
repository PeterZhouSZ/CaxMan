cd ~/CaxMan/

git pull

cd WP3_WS_IMATI/target/

rm -r *

cd ..

mvn package

cd target

asadmin undeploy WP3_WS_IMATI-1.0

asadmin deploy WP3_WS_IMATI-1.0.war
