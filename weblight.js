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

function hexToRgb(hex) {
  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result ? {
    r: parseInt(result[1], 16),
    g: parseInt(result[2], 16),
    b: parseInt(result[3], 16)
  } : null;
}

function setDeviceColor(device, r, g, b) {
  var rgb = new Uint8Array(3);
  rgb[0] = r;
  rgb[1] = g;
  rgb[2] = b;

  device.controlTransferOut({
    'requestType': 'vendor',
    'recipient': 'device',
    'request': 0x01,
    'value': 0x00,
    'index': 0x00}, rgb)
	  .then(o => {}, e => {console.log(e); disconnectDevice(device.guid);});
}

function handleColorChange(device) {
  var picker = device.element.getElementsByClassName("lightPicker")[0];
  var color = hexToRgb(picker.value);
  setDeviceColor(device, color.r, color.g, color.b);
}


function setElementDeviceInfo(e, text) {
  e.getElementsByClassName("lightTitle")[0].innerText = text;
}

function setElementColor(e, color) {
    e.getElementsByClassName("lightStatus")[0].innerText = color;
}

var BRIGHTNESS = 64;
function connectDevice(device) {
  var eTemplate = document.getElementById("lightCardTemplate");
  var e = eTemplate.cloneNode(true);
  lightsParent.appendChild(e);
  console.log("new element", e);
  e.removeAttribute("id");
  e.style.display = "block";
  console.log("see", e.childNodes);

  var picker = e.getElementsByClassName("lightPicker")[0];
  picker.addEventListener("change",
                          handleColorChange.bind(this, device),
                          false);

  e.getElementsByClassName("lightRed")[0].addEventListener(
    "click",
    setDeviceColor.bind(this, device, BRIGHTNESS, 0, 0)
  );
  e.getElementsByClassName("lightGreen")[0].addEventListener(
    "click",
    setDeviceColor.bind(this, device, 0, BRIGHTNESS, 0)
  );
  e.getElementsByClassName("lightBlue")[0].addEventListener(
    "click",
    setDeviceColor.bind(this, device, 0, 0, BRIGHTNESS)
  );
  e.getElementsByClassName("lightOff")[0].addEventListener(
    "click",
    setDeviceColor.bind(this, device, 0, 0, 0)
  );

  device.element = e;
  var s = device.device_.productName + "\n" +
    device.device_.serialNumber;
  setElementDeviceInfo(device.element, s);
  device.connect()
    .then(logDeviceStrings(device))
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
