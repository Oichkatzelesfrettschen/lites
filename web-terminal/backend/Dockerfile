FROM debian:bookworm-slim

# Install QEMU and dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        qemu-system-x86 \
        qemu-utils \
        curl \
        ca-certificates \
        && rm -rf /var/lib/apt/lists/*

# Create directory for QEMU images
RUN mkdir -p /lites

# Copy entrypoint script
COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh

# Expose ports
# 7681: WebSocket serial console
# 2222: SSH forwarding
# 5900: VNC (optional)
EXPOSE 7681 2222 5900

# Set working directory
WORKDIR /lites

# Run entrypoint
CMD ["/usr/local/bin/entrypoint.sh"]
