var webusb = {};

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

(function() {
  'use strict';

  webusb.getDevices = function() {
    return navigator.usb.getDevices().then(devices => {
      console.log(devices);
      return devices.map(device => new webusb.Device(device));
    });
  };

  webusb.Device = function(device) {
    this.device_ = device;
  };

  webusb.Device.prototype.connect = function() {
    let readLoop = () => {
      this.device_.transferIn(2, 64).then(result => {
        this.onReceive(result.data);
        readLoop();
      }, error => {
        this.onReceiveError(error);
      });
    };

    return this.device_.open()
        .then(() => this.device_.getConfiguration()
            .then(config => {
              if (config.configurationValue == 1) {
                return Promise.resolve();
              } else {
                return Promise.reject("Need to setConfiguration(1).");
              }
            })
            .catch(error => this.device_.setConfiguration(1)))
        .then(() => this.device_.claimInterface(0))
        .then(() => this.device_.controlTransferOut({
            'requestType': 'class',
            'recipient': 'interface',
            'request': 0x22,
            'value': 0x01,
            'index': 0x00}))
        .then(() => {
          readLoop();
        });
  };

  webusb.Device.prototype.disconnect = function() {
    return this.device_.controlTransferOut({
            'requestType': 'class',
            'recipient': 'interface',
            'request': 0x22,
            'value': 0x00,
            'index': 0x00})
        .then(() => this.device_.close());
  };

  webusb.Device.prototype.controlTransferOut = function(setup, data) {
    return this.device_.controlTransferOut(setup, data);
  };

  webusb.Device.prototype.controlTransferIn = function(setup, length) {
    return this.device_.controlTransferIn(setup, length);
  };

})();

function start() {
  webusb.getDevices().then(devices => {
    'use strict';

    if (devices.length == 0) {
      console.log("no device found");
    } else {
      let device = devices[0];
      device.connect().then(() => {
        device.controlTransferIn({
          'requestType': 'standard',
          'recipient': 'device',
          'request': 6,
          'value': 0x0302,
          'index': 0x01}, 64).then(o => {
            console.log("Found device calling itself", ab2str(o.data));
          });

        var cycle = 0;
        var name = '';
        var doLight = function() {
          let rgb = new Uint8Array(3);

          switch (cycle) {
          case 0:
            rgb[0] = 0x80;
            rgb[1] = 0x00;
            rgb[2] = 0x00;
            name = 'red';
            break;
          case 1:
            rgb[0] = 0x00;
            rgb[1] = 0x80;
            rgb[2] = 0x00;
            name = 'green';
            break;
          case 2:
            rgb[0] = 0x00;
            rgb[1] = 0x00;
            rgb[2] = 0x80;
            name = 'blue';
            break;
          }
          if (++cycle > 2) {
            cycle = 0;
          }
          document.getElementById('color').innerText = name;
          device.controlTransferOut({
            'requestType': 'vendor',
            'recipient': 'device',
            'request': 0x01,
            'value': 0x00,
            'index': 0x00}, rgb).then(o => {
              console.log(name);
            });

        };
        doLight();

        window.setInterval(doLight, 1000);
      });
    }
  });
}

document.addEventListener('DOMContentLoaded', start, false);
