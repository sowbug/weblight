var webusb = {};
var lightsParent;

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint16Array(buf.slice(2)));
}

(function() {
  'use strict';

  webusb.devices = {};

  webusb.getDevices = function() {
    return navigator.usb.getDevices().then(devices => {
      return devices.map(device => new webusb.Device(device));
    });
  };

  webusb.Device = function(device) {
    this.device_ = device;
    webusb.devices[device.guid] = this;
  };

  webusb.deleteDevice = function(device) {
    delete webusb.devices[device.device_.guid];
  };

  webusb.getDeviceFromGuid = function(guid) {
    return webusb.devices[guid];
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

function ensureHTTPS() {
  if (window.location.protocol != "https:" &&
      window.location.host == "sowbug.github.io") {
    window.location.href = "https:" +
        window.location.href.substring(window.location.protocol.length);
  }
}

function installServiceWorker() {
  if ('serviceWorker' in navigator) {
    // Override the default scope of '/' with './', so that the registration
    // applies to the current directory and everything underneath it.
    navigator.serviceWorker.register('service-worker.js', {scope: './'})
	.then(function(registration) {
	  // At this point, registration has taken place. The service worker
	  // will not handle requests until this page and any other instances
	  // of this page (in other tabs, etc.) have been closed/reloaded.
	  console.log("document.querySelector('#status').textContent = 'success';");
	}).catch(function(error) {
	  // Something went wrong during registration. The service-worker.js
	  // file might be unavailable or contain a syntax error.
	  console.log("document.querySelector('#status').textContent = error;");
	});
  } else {
    // The current browser doesn't support service workers.
    var aElement = document.createElement('a');
    aElement.href = 'http://www.chromium.org/blink/serviceworker/service-worker-faq';
    aElement.textContent = 'Service Worker Unavailable';
    document.querySelector('#status').appendChild(aElement);
  }
}

function logDeviceStrings(device) {
  console.log("Connection:",
	      device.device_.manufacturerName,
	      device.device_.productName,
	      device.device_.serialNumber);
}

function blinkLights(device) {
  var rgb = new Uint8Array(3);

  var brightness = 0x20;
  var name = '';
  switch (device.cycle) {
    case 0:
    rgb[0] = brightness;
    rgb[1] = 0x00;
    rgb[2] = 0x00;
    name = 'red';
    break;
    case 1:
      rgb[0] = 0x00;
      rgb[1] = brightness;
      rgb[2] = 0x00;
      name = 'green';
      break;
      case 2:
      rgb[0] = 0x00;
      rgb[1] = 0x00;
      rgb[2] = brightness;
      name = 'blue';
      break;
  }
  setElementColor(device.element, name);
  if (++device.cycle > 2) {
    device.cycle = 0;
  }
  device.controlTransferOut({
    'requestType': 'vendor',
    'recipient': 'device',
    'request': 0x01,
    'value': 0x00,
    'index': 0x00}, rgb)
	.then(o => {}, e => {console.log(e); disconnectDevice(device.guid);});
}

function startBlinkLights(device) {
  console.log("startBlinkLights", device);

  device.cycle = 0;
  blinkLights(device);
  device.intervalId = window.setInterval(blinkLights.bind(this, device),
                                         Math.random() * 1750 + 250);
}

function setElementDeviceInfo(e, text) {
  e.childNodes[0].innerText = text;
}

function setElementColor(e, color) {
  e.childNodes[1].innerText = color;
}

function connectDevice(device) {
  var e = document.createElement("div");
  e.className = "card-square mdl-card mdl-shadow--2dp centered";
  var eId = document.createElement("div");
  eId.className = "mdl-card__title mdl-card--expand";
  var eColor = document.createElement("div");
  eColor.className = "mdl-card__supporting-text";
  e.appendChild(eId);
  e.appendChild(eColor);
  lightsParent.appendChild(e);
  device.element = e;
  var s = device.device_.productName + "\n" +
      device.device_.serialNumber;
  setElementDeviceInfo(device.element, s);
  device.connect()
      .then(logDeviceStrings(device))
      .then(startBlinkLights(device))
      .then(function() { console.log("connected", device) });
}

function handleConnectEvent(event) {
  var rawDevice = event.device;
  var guid = rawDevice.guid;
  console.log('connect event', rawDevice, guid);
  var device = new webusb.Device(rawDevice);
  connectDevice(device);
}

function cleanUpDevice(device) {
  clearInterval(device.intervalId);
  webusb.deleteDevice(device);
}

function disconnectDevice(guid) {
  if (!guid in webusb.devices) {
    console.log(guid, "not known");
    return;
  }

  var device = webusb.getDeviceFromGuid(guid);
  if (device) {  // This can fail if the I/O code already threw an exception
    console.log("removing!");
    lightsParent.removeChild(device.element);
    device.disconnect()
	.then(s => {
	  console.log("disconnected", device);
	  cleanUpDevice(device);
	}, e => {
	  console.log("nothing to disconnect", device);
	  cleanUpDevice(device);
	});
  }
}

function handleDisconnectEvent(event) {
  var rawDevice = event.device;
  var guid = rawDevice.guid;
  console.log('disconnect event', rawDevice, guid);
  disconnectDevice(guid);

  //    color.innerText = 'no device';
}

function registerEventListeners() {
  navigator.usb.addEventListener('connect', handleConnectEvent);
  navigator.usb.addEventListener('disconnect', handleDisconnectEvent);
}

function startInitialConnections() {
  webusb.getDevices().then(devices => {
    console.log(devices);
    for (var i in devices) {
      var device = devices[i];
      connectDevice(device);
    }
  });
}

function start() {
  ensureHTTPS();
  installServiceWorker();
  registerEventListeners();

  lightsParent = document.getElementById("lightsParent");

  startInitialConnections();
}

document.addEventListener('DOMContentLoaded', start, false);
