# run as sudo
cd /usr/local
wget https://go.dev/dl/go1.17.3.linux-amd64.tar.gz
tar -xvzf go1.17.3.linux-amd64.tar.gz

echo export GOROOT=\"/usr/local/go\" >> ~/.bashrc
echo export GOPATH=\"\$HOME/go\" >> ~/.bashrc
echo export PATH=\"\$PATH:/usr/local/go:\$GOPATH/bin\" >> ~/.bashrc

update-alternatives --install "/usr/bin/go" "go" "$GOROOT/bin/go" 0
update-alternatives --set go $GOROOT/bin/go