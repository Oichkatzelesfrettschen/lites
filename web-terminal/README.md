# Lites i386 Web Terminal

This directory contains a complete web-accessible Lites i386 instance using GitHub Pages for the frontend and QEMU with WebSocket serial console for the backend.

## Architecture

```
┌──────────────────┐         WebSocket          ┌─────────────────────┐
│  GitHub Pages    │◄──────── (ws://) ─────────►│  QEMU Backend       │
│  (Static HTML/JS)│                             │  (VPS/Server)       │
│                  │                             │                     │
│  - xterm.js      │                             │  - Debian GNU/Hurd  │
│  - Web UI        │                             │  - Serial Console   │
│  - Connection    │                             │  - SSH Access       │
└──────────────────┘                             └─────────────────────┘
```

## Components

### Frontend (`index.html`)
- **xterm.js**: Full-featured terminal emulator in the browser
- **WebSocket Client**: Connects to QEMU's serial console
- **Responsive UI**: Works on desktop and mobile
- **Status Monitoring**: Connection state and logs
- **Customizable**: Easy to theme and extend

### Backend (`backend/`)
- **Dockerfile**: QEMU image with WebSocket support
- **entrypoint.sh**: Launch script with configuration
- **docker-compose.yml**: Easy deployment configuration

## Quick Start

### 1. Deploy Frontend to GitHub Pages

```bash
# Copy index.html to your GitHub Pages repository
cp web-terminal/index.html /path/to/your-pages-repo/lites-terminal.html

# Or enable Pages for this repo
# Settings → Pages → Source: main branch → /web-terminal directory
```

### 2. Set Up Backend

#### Option A: Docker Compose (Recommended)

```bash
cd web-terminal

# Create images directory
mkdir -p images

# Place your QEMU image
# Download or build a Debian GNU/Hurd image and place it at:
# images/hurd.qcow2

# Start the backend
docker compose up -d

# View logs
docker compose logs -f
```

#### Option B: Docker Run

```bash
# Build image
docker build -t lites-web-backend ./backend

# Run with image mounted
docker run -d \
  --name lites-web \
  --privileged \
  --device /dev/kvm:/dev/kvm \
  -v /path/to/hurd.qcow2:/hurd/hurd.qcow2:ro \
  -p 7681:7681 \
  -p 2222:2222 \
  -e HURD_RAM=4096 \
  -e HURD_CPUS=2 \
  lites-web-backend
```

#### Option C: Manual QEMU

```bash
qemu-system-i386 \
  -accel kvm -cpu host \
  -machine pc -m 4096 -smp 2 \
  -device ich9-ahci,id=ach0 \
  -drive if=none,file=/path/to/hurd.qcow2,format=qcow2,id=d0 \
  -device ide-hd,drive=d0,bus=ach0.0 \
  -device e1000,netdev=net0 \
  -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  -chardev socket,id=ws0,host=0.0.0.0,port=7681,server=on,websocket=on \
  -serial chardev:ws0 \
  -nographic
```

### 3. Connect

1. Open the GitHub Pages URL (e.g., `https://yourusername.github.io/lites-terminal.html`)
2. Enter WebSocket URL: `ws://your-server-ip:7681`
3. Click "Connect"
4. Login to Debian GNU/Hurd

## Configuration

### Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `HURD_IMG` | `/hurd/hurd.qcow2` | Path to QEMU image |
| `HURD_RAM` | `4096` | RAM in MB |
| `HURD_CPUS` | `1` | Number of CPU cores |
| `WS_PORT` | `7681` | WebSocket port |
| `SSH_FWD_HOSTPORT` | `2222` | SSH forwarding port |

### Frontend Configuration

Edit `index.html` to change:
- Default WebSocket URL
- Terminal theme colors
- Font family and size
- UI layout and styling

## Creating a Debian GNU/Hurd Image

If you don't have a QEMU image, create one:

```bash
# Download Debian GNU/Hurd installer
wget https://cdimage.debian.org/cdimage/ports/latest/hurd-i386/debian-hurd.img.tar.gz
tar xzf debian-hurd.img.tar.gz

# Or build from scratch
qemu-img create -f qcow2 hurd.qcow2 20G

# Install from ISO
qemu-system-i386 \
  -m 4096 \
  -cdrom debian-hurd-installer.iso \
  -hda hurd.qcow2 \
  -boot d

# After installation, test it
qemu-system-i386 -m 4096 -hda hurd.qcow2 -nographic
```

## Networking and Security

### Production Deployment

#### 1. HTTPS/WSS with Cloudflare Tunnel

```bash
# Install cloudflared
wget https://github.com/cloudflare/cloudflared/releases/latest/download/cloudflared-linux-amd64
chmod +x cloudflared-linux-amd64
sudo mv cloudflared-linux-amd64 /usr/local/bin/cloudflared

# Login
cloudflared tunnel login

# Create tunnel
cloudflared tunnel create lites-terminal

# Configure
cat > ~/.cloudflared/config.yml <<EOF
tunnel: YOUR-TUNNEL-ID
credentials-file: /home/user/.cloudflared/YOUR-TUNNEL-ID.json

ingress:
  - hostname: lites.yourdomain.com
    service: ws://localhost:7681
  - service: http_status:404
EOF

# Run tunnel
cloudflared tunnel run lites-terminal
```

Now update frontend URL to: `wss://lites.yourdomain.com`

#### 2. Nginx Reverse Proxy

```nginx
server {
    listen 443 ssl http2;
    server_name lites.yourdomain.com;
    
    ssl_certificate /path/to/cert.pem;
    ssl_certificate_key /path/to/key.pem;
    
    location / {
        proxy_pass http://localhost:7681;
        proxy_http_version 1.1;
        proxy_set_header Upgrade $http_upgrade;
        proxy_set_header Connection "Upgrade";
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_read_timeout 86400;
    }
}
```

### Security Considerations

⚠️ **Important**: The serial console provides direct access to the system. For public deployments:

1. **Add Authentication**: Implement a WebSocket authentication layer
2. **Use TLS/SSL**: Always use `wss://` (not `ws://`) in production
3. **Firewall**: Restrict access to known IPs
4. **Read-Only Image**: Mount QEMU image read-only if possible
5. **Resource Limits**: Set CPU/memory limits in Docker
6. **Monitoring**: Log all connections and activities

Example authentication wrapper:

```javascript
// In index.html, before connecting:
const authToken = prompt('Enter access token:');
ws = new WebSocket(`${url}?token=${authToken}`);
```

## Troubleshooting

### WebSocket Connection Fails

**Symptom**: "Connection error" in browser

**Solutions**:
1. Check backend is running: `docker ps`
2. Verify port is open: `netstat -tuln | grep 7681`
3. Check firewall rules: `sudo ufw status`
4. Test with: `wscat -c ws://localhost:7681`
5. Check browser console for errors

### QEMU Won't Start

**Symptom**: Container exits immediately

**Solutions**:
1. Check image exists: `ls -lh images/hurd.qcow2`
2. View logs: `docker compose logs`
3. Verify permissions: `chmod 644 images/hurd.qcow2`
4. Test image: `qemu-img info images/hurd.qcow2`

### No KVM Acceleration

**Symptom**: "KVM not available" in logs

**Solutions**:
1. Check KVM module: `lsmod | grep kvm`
2. Verify device: `ls -l /dev/kvm`
3. Add user to kvm group: `sudo usermod -aG kvm $USER`
4. Run with `--privileged` flag

### Terminal Not Responsive

**Symptom**: Can't type in terminal

**Solutions**:
1. Check WebSocket is connected (green status)
2. Refresh page
3. Check QEMU is running
4. Try reconnecting
5. Check browser console for JavaScript errors

## Advanced Configuration

### Custom Terminal Theme

Edit `index.html` theme section:

```javascript
theme: {
    background: '#1e1e1e',     // Dark background
    foreground: '#d4d4d4',     // Light text
    cursor: '#00ff00',         // Green cursor
    // ... customize all colors
}
```

### Multiple Instances

Run multiple QEMU instances on different ports:

```yaml
# docker-compose.yml
services:
  lites-1:
    extends: lites-web-backend
    ports:
      - "7681:7681"
  
  lites-2:
    extends: lites-web-backend
    ports:
      - "7682:7681"
```

### VNC Support

Enable VNC alongside serial:

```bash
# Add to QEMU command
-vnc :0,websocket=on

# Connect with noVNC
# https://github.com/novnc/noVNC
```

### SSH Access

SSH is forwarded to port 2222:

```bash
ssh -p 2222 root@localhost
```

## Performance Optimization

1. **Enable KVM**: 10-50x faster than TCG
2. **Increase RAM**: Set `HURD_RAM=8192` for better performance
3. **More CPUs**: Set `HURD_CPUS=4` if available
4. **SSD Storage**: Use SSD for QEMU image
5. **Dedicated Server**: Deploy on VPS with good specs

## Integration with Existing Docker Setup

This web terminal can work alongside the existing `docker/` i386 development environment:

```bash
# Use the i386 dev environment to build Lites
cd docker
make -f ../Makefile.docker docker-build-lites

# Export the built image for web terminal
# (This would require additional steps to create a bootable image)
```

## API and Automation

### Programmatic Connection

```javascript
// Connect programmatically
const ws = new WebSocket('ws://localhost:7681');

ws.onopen = () => {
    console.log('Connected');
    // Send commands
    ws.send('uname -a\n');
};

ws.onmessage = (event) => {
    console.log('Output:', event.data);
};
```

### Health Check Endpoint

Add a simple HTTP health check (requires modification):

```bash
# Check if WebSocket is listening
curl -i -N -H "Connection: Upgrade" \
  -H "Upgrade: websocket" \
  -H "Sec-WebSocket-Version: 13" \
  -H "Sec-WebSocket-Key: test" \
  http://localhost:7681
```

## License

Same as the main Lites repository. See [LICENSE](../LICENSE).

## References

- [xterm.js Documentation](https://xtermjs.org/)
- [QEMU WebSocket Documentation](https://www.qemu.org/docs/master/interop/live-block-operations.html)
- [Debian GNU/Hurd](https://www.debian.org/ports/hurd/)
- [GitHub Pages](https://pages.github.com/)
- [Cloudflare Tunnel](https://developers.cloudflare.com/cloudflare-one/connections/connect-apps/)

## Support

For issues or questions:
1. Check the [Troubleshooting](#troubleshooting) section
2. Review [Main Docker Documentation](../docker/README.md)
3. Open an issue with the `web-terminal` label

---

**Ready to deploy?** Follow the Quick Start guide above and you'll have a web-accessible Lites instance in minutes!
