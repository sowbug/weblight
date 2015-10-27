var webusb = {};

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint8Array(buf));
}

(function() {
  'use strict';

  webusb.getDevices = function() {
    return navigator.usb.getDevices().then(devices => {
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
        console.log(error);
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
        'index': 0x00}));
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
  'use strict';
  
  if (window.location.protocol != "https:" &&
      window.location.hostname != "localhost") {
    window.location.href = "https:" +
        window.location.href.substring(window.location.protocol.length);
  }

  var device = null;
  var intervalId = 0;
  var status = document.getElementById('color');

  navigator.usb.addEventListener('disconnect', function(event) {
    console.log('disconnect event', event.device, device);
    if (!device) {
      return;
    }
    if (device.device_.guid == event.device.guid && intervalId > 0) {
      console.log('stopping');
      clearInterval(intervalId);
      intervalId = 0;

      status.innerText = 'no device';
      device.disconnect();
    }
  });

  webusb.getDevices().then(devices => {

    if (devices.length == 0) {
      status.innerText = 'no device';
      console.log("no device found");
      navigator.usb.addEventListener('connect', function() {
        console.log('connect event', this);

        // TODO(miket): this is horrible. I need to study up on
        // promises.
        start();
      });
    } else {
      device = devices[0];

      function logManufacturer(dev) {
        return new Promise(function(f, r) {
          dev.controlTransferIn({
            'requestType': 'standard',
            'recipient': 'device',
            'request': 6,
            'value': 0x0301,
            'index': 0
          }, 64)
            .then(o => {
              console.log("Manufacturer", ab2str(o.data));
              f(o.data);
            })
            .catch(o => {
              console.log("Manufacturer", o);
            });
        });
      }

      function logProduct(dev) {
        return new Promise(function(f, r) {
          dev.controlTransferIn({
            'requestType': 'standard',
            'recipient': 'device',
            'request': 6,
            'value': 0x0302,
            'index': 0
          }, 64)
            .then(o => {
              console.log("Product", ab2str(o.data));
              f(o.data);
            })
            .catch(o => {
              console.log("Product", o);
            });
        });
      }

      function logSerialNumber(dev) {
        return new Promise(function(f, r) {
          dev.controlTransferIn({
            'requestType': 'standard',
            'recipient': 'device',
            'request': 6,
            'value': 0x0303,
            'index': 0
          }, 64).then(o => {
            console.log("Serial number", ab2str(o.data));
            f(o.data);
          })
            .catch(o => {
              console.log("Serial number", o);
            });
        });
      }

      device.connect()
        .then(logManufacturer.bind(this, device))
        .then(logProduct.bind(this, device))
        .then(logSerialNumber.bind(this, device))
        .then(() => {

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
            status.innerText = name;
            device.controlTransferOut({
              'requestType': 'vendor',
              'recipient': 'device',
              'request': 0x01,
              'value': 0x00,
              'index': 0x00}, rgb).then(o => {}, e => {console.log(e);});

          };
          doLight();

          intervalId = window.setInterval(doLight, 1000);
        });
    }
  });
}

document.addEventListener('DOMContentLoaded', start, false);
