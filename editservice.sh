#! /bin/sh
sudo nano /etc/systemd/system/domoticz.service
sudo systemctl daemon-reload
sudo systemctl enable domoticz.service
