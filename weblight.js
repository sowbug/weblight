var webusb = {};
var lightsParent;

function ab2str(buf) {
  return String.fromCharCode.apply(null, new Uint16Array(buf.slice(2)));
}

(function() {
  'use strict';

  webusb.devices = {};

  function findOrCreateDevice(rawDevice) {
    let device = webusb.getDevice(rawDevice);
    if (device === undefined)
      device = new webusb.Device(rawDevice);
    return device;
  }

  webusb.getDevices = function() {
    return navigator.usb.getDevices().then(devices => {
      return devices.map(device => findOrCreateDevice(device));
    });
  };

  webusb.requestDevice = function() {
    var filters = [
      { vendorId: 0x1209, productId: 0xa800 }
    ];
    return navigator.usb.requestDevice({filters: filters}).then(device => {
      return findOrCreateDevice(device);
    });
  };

  webusb.Device = function(device) {
    this.device_ = device;
    if (device.guid === undefined)
      webusb.devices[device] = this
    else
      webusb.devices[device.guid] = this;
  };

  webusb.deleteDevice = function(device) {
    if (device.device_.guid === undefined)
      delete webusb.devices[device.device_];
    else
      delete webusb.devices[device.device_.guid];
  };

  webusb.getDevice = function(device) {
    if (device.guid === undefined)
      return webusb.devices[device];
    else
      return webusb.devices[device.guid];
  };

  webusb.Device.prototype.connect = function() {
    return this.device_.open()
      .then(() => {
        if (this.device_.configuration === null) {
          return this.device_.selectConfiguration(1);
        }
      })
      .then(() => this.device_.claimInterface(0));
  };

  webusb.Device.prototype.disconnect = function() {
    return this.device_.close();
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

function componentToHex(c) {
  var hex = c.toString(16);
  return hex.length == 1 ? "0" + hex : hex;
}

function rgbToHex(r, g, b) {
  return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
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
	  .then(o => {
      device.element.getElementsByClassName(
        "lightPicker")[0].value = rgbToHex(r, g, b);
    }, e => {
      console.log(e); disconnectDevice(device);
    });
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
  e.removeAttribute("id");
  e.style.display = "block";

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
  e.getElementsByClassName("lightWhite")[0].addEventListener(
    "click",
    setDeviceColor.bind(this, device, 255, 255, 255)
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
  console.log('connect event', rawDevice);
  var device = new webusb.Device(rawDevice);
  connectDevice(device);
}

function cleanUpDevice(device) {
  clearInterval(device.intervalId);
  webusb.deleteDevice(device);
}

function disconnectDevice(rawDevice) {
  var device = webusb.getDevice(rawDevice);
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
  console.log('disconnect event', event.device);
  disconnectDevice(event.device);

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

function requestConnection(event) {
  webusb.requestDevice().then(device => {
    console.log(device);
    connectDevice(device);
  });
  event.preventDefault();
}

function start() {
  ensureHTTPS();
  installServiceWorker();
  registerEventListeners();

  var lightsConnect = document.getElementById("lightConnect");
  lightsConnect.addEventListener("click", requestConnection);

  lightsParent = document.getElementById("lightsParent");

  startInitialConnections();
}

document.addEventListener('DOMContentLoaded', start, false);
