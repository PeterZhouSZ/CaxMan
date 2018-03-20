cd ~/CaxMan/

git pull

cd WP3_WS_IMATI/target/

rm -r *

cd ..

echo "Copying service executable ... "

CPP_FOLDER="cpp/"

mkdir -p $CPP_FOLDER

cp -r /root/CaxMan/detect_voids_service $CPP_FOLDER
cp -r /root/CaxMan/global_checks $CPP_FOLDER
cp -r /root/CaxMan/integrity_checks $CPP_FOLDER
cp -r /root/CaxMan/orientation_service $CPP_FOLDER
cp -r /root/CaxMan/slice2mesh $CPP_FOLDER
cp -r /root/CaxMan/stl2cli $CPP_FOLDER
cp -r /root/CaxMan/support_structures $CPP_FOLDER
cp -r /root/CaxMan/thinwalls_and_cavities $CPP_FOLDER

echo "Copying libs ... " 

LIB_FOLDER="libs/"
mkdir -p $LIB_FOLDER

cp -r /usr/local/lib/ $LIB_FOLDER

echo "Copying third_party libs ... " 

TP_LIB_FOLDER="third_party_libs/"
mkdir -p $TP_LIB_FOLDER

cp -r /root/third_party_libs $TP_LIB_FOLDER

echo "Stopping and killing existing docker images ... "

docker stop $(docker ps -a | grep 'wp3_services' |awk '{print $1}') &>> /root/docker_log
docker rm $(docker ps -a | grep 'wp3_services' |awk '{print $1}') &>> /root/docker_log

echo "Rebuilding services"

./build.sh

printf "\nDOCKER_CONTAINER_ID = "
./run.sh

printf "\nRestarting Glassfish in background ..."

until $(curl --output /dev/null --silent --head --fail https://caxman.clesgo.net/imati/wp3/orientation_optimization?wsdl); 
do 
	printf '.';
	sleep 1;
done

printf "\n\n Deployed Web services! \n\n"

#printf "\n\nTo check Glassfish log, use :\n  docker logs DOCKER_CONTAINER_ID\n\n\n"
