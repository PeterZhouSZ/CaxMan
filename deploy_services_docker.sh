cd ~/CaxMan/

git pull

cd WP3_WS_IMATI/target/

rm -r *

cd ..

tmux kill-session -t deploy_services &> /root/docker_log

echo "Stopping and killing existing docker images ... "

docker stop $(docker ps -a | grep 'wp3_services' |awk '{print $1}') &>> /root/docker_log
docker rm $(docker ps -a | grep 'wp3_services' |awk '{print $1}') &>> /root/docker_log

echo "Rebuilding services"

./build.sh

tmux new-session -d -s deploy_services

echo "Restarting Glassfish .. Services will be available in a while ..."

tmux send-key ./run.sh C-m
tmux detach -s deploy_services
