FROM ghcr.io/userver-framework/ubuntu-22.04-userver:latest

# Устанавливаем необходимые инструменты для разработки
RUN apt-get update && apt-get install -y \
    vim \
    nano \
    git \
    curl \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

# Устанавливаем переменные окружения для userver
ENV USERVER_ROOT=/usr

CMD ["/bin/bash"]
