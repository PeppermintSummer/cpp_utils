# ==================================================================
# module list
# ------------------------------------------------------------------
# python        3.8    (apt)
# pytorch       latest (pip)
# ==================================================================
# sudo docker build -t guyu_cu100:v1 . 构建镜像
# docker pull nvidia/cuda:10.0-devel-ubuntu18.04
# docker pull nvidia/cuda:10.0-devel-ubuntu18.04
# docker run -itd --runtime=nvidia --gpus=all -e NVIDIA_DRIVER_CAPABILITIES=compute,utility,video,graphics -v /root/Documents/guyu:/data/ --name dmrnet --privileged=true -v /dev/shm:/dev/shm a411307c4f6b /bin/bash
# -it d; -v 磁盘挂载 本地绝对路径:容器的绝对路径 --name -v /dev/shm:/dev/shm 共享内存; a411307c4f6b image ID; 执行命令
# 他会创建一个容器出来,本身带着id，然后docker ps -a 即可查看
# 进入容器
# docker exec -it container-id (f34d733ff398) /bin/bash
# python
# pip install
# docker export 容器导出

FROM nvidia/cuda:10.0-devel-ubuntu18.04
ENV LANG C.UTF-8
RUN APT_INSTALL="apt-get install -y --no-install-recommends" && \
    PIP_INSTALL="python -m pip --no-cache-dir install --upgrade" && \
    GIT_CLONE="git clone --depth 10" && \

    rm -rf /var/lib/apt/lists/* \
           /etc/apt/sources.list.d/cuda.list \
           /etc/apt/sources.list.d/nvidia-ml.list && \

    apt-get update && \

# ==================================================================
# tools
# ------------------------------------------------------------------

    DEBIAN_FRONTEND=noninteractive $APT_INSTALL \
        build-essential \
        apt-utils \
        ca-certificates \
        wget \
        git \
        vim \
        libssl-dev \
        curl \
        unzip \
        unrar \
        cmake \
        && \

# ==================================================================
# python
# ------------------------------------------------------------------

    apt-get update && \
    DEBIAN_FRONTEND=noninteractive $APT_INSTALL \
        python3.8 \
        python3.8-dev \
        python3.8-distutils \
        && \
    wget -O ~/get-pip.py https://bootstrap.pypa.io/get-pip.py && \
    python3.8 ~/get-pip.py && \
    ln -s /usr/bin/python3.8 /usr/local/bin/python && \
    $PIP_INSTALL \
        numpy \
        scipy \
        pandas \
        scikit-image \
        scikit-learn \
        matplotlib \
        Cython \
        tqdm \
        && \
# ==================================================================
# pytorch
# ------------------------------------------------------------------
#pip3 install torch==1.8.2+cu102 torchvision==0.9.2+cu102 torchaudio==0.8.2 -f https://download.pytorch.org/whl/lts/1.8/torch_lts.html
    $PIP_INSTALL \
        future \
        numpy \
        protobuf \
        enum34 \
        pyyaml \
        typing \
        && \
    $PIP_INSTALL \
        --pre torch==1.4.0+cu100 torchvision==0.5.0+cu100 -f \
        https://download.pytorch.org/whl/torch_stable.html \
        && \

# ==================================================================
# config & cleanup
# ------------------------------------------------------------------

    ldconfig && \
    apt-get clean && \
    apt-get autoremove && \
    rm -rf /var/lib/apt/lists/* /tmp/* ~/*