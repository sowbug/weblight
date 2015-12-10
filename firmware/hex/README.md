The files in this directory are prebuilt hex files. To reconstruct them:

* `full-[version].hex`: follow the instructions in the top-level
  README for creating a mass-production image.

* `t85_default.hex`: Visit [the original micronucleus
  repository](https://github.com/micronucleus/micronucleus/tree/master/firmware/releases)
  and see what's there.

* `t85_watchdog_reset.hex`: clone the micronucleus repo and build the
  default target in the `firmware` directory, but with
  [ENTRYMODE](https://github.com/micronucleus/micronucleus/blob/master/firmware/configuration/t85_default/bootloaderconfig.h#L110)
  set to `ENTRY_WATCHDOG` rather than `ENTRY_ALWAYS`

* `t85_watchdog_or_ext_reset.hex`: same as above but with [this
  patch](https://github.com/sowbug/micronucleus/commit/b803a3a8952fb07733bb7993ce71023a4ca8f0fb)
  that introduces a new entry mode called
  `ENTRY_WATCHDOG_AND_EXT`. (OR is more appropriate than AND, but I
  wasn't thinking straight at the time.)
