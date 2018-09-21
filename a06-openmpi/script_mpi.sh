#!/bin/bash

# verifica se o usuário tem privilégio de root
if [ ${USER} != "root" ]; then
   echo "[ERROR] Voce não tem os privilegios necessarios para executar esse script!" &&
    exit
fi

wget https://download.open-mpi.org/release/open-mpi/v3.1/openmpi-3.1.2.tar.gz
tar -xvf openmpi-3.1.2.tar.gz
cd openmpi-3.1.2
bash configure --prefix="/opt/openmpi"
make -s -i
make install -s -i
echo 'export MPI_HOME=/opt/openmpi\n export PATH=$PATH:$MPI_HOME/bin' >> /etc/profile.d/openmpi.sh
echo 'export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/openmpi/lib/' >> /etc/profile.d/openmpi_libs.sh
cd ..
rm -rf openmpi-3.1.2
source ~/.bashrc
ldconfig
