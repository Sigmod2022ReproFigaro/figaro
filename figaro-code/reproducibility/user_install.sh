apt --yes update && apt --yes upgrade
apt install --yes sudo
adduser zivanovic --gecos '' --disabled-password
usermod -aG sudo zivanovic
echo "zivanovic  ALL=(ALL) NOPASSWD:ALL" | sudo tee /etc/sudoers.d/zivanovic
