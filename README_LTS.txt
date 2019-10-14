This patch to Drive-Trust-Alliance/sedutil provides the following updates:
  - Update Buildroot2 to 2019.02.6 (latest LTS version as of 14 October).
  - Update linux kernel version to 4.14.146 (4.14.x and 4.17.x are current LTS
    release streams.)

The primary driving requirements for this were:
  1.  Move to an LTS kernel version with the expectation that this could
      fix some issues with USB support in the linuxpba utility, as well as
      providing better support for newer Intel and AMD chips (particularly
      Ryzen 2xxx and 3xxx chips).  ==> Yes, this update fixes those issues.
  2.  Move to a Linux kernel version that allows compilation with Gnu GCC 8
      and supports the full set of Spectre/Meltdown fixes.  For some of us
      who might want to use SEDutil in an enterprise environment, it's
      easier to be able to clearly state that Spectre fixes are present,
      than to explain why there's really no attack surface from the older
      Linux kernel
  3.  Move incrementally to the "next" Linux LTS kernel, rather than the
      absolute newest, given that the 4.11.8 kernel was a much older vintage.

Along the way with Linux kernel updates, there were two other discoveries
that affected the final upgrade path:
  A.  It turns out Buildroot2 also has LTS versions, specifically the 20xx.02
      releases are the LTS ones.  My original selection was 2018.08 but that
      turned into 2019.02 in order to get to an LTS buildroot.
  B.  As of 2018.11-rc1 SEDutil became an "official" part of Buildroot2 -
      which means that when building newer Buildroot2 versions/targets, it
      becomes necessary to override the Buildroot2 "official" source for
      SEDutil with our own updated/customized tarball.  (Otherwise we end
      up with DTA SEDutil plus Buildroot2 patches but no other local
      patches that might be intentional/needed...)

For anyone who wants to experiment with a newer Buildroot2 version or Linux
kernel version:
  - Buildroot2 target version is determined by images/conf
    Any changes to the Buildroot2 target version may require a new buildroot
    .config file to be created.  The actual files from the source tarball are
    kept in images/buildroot/{32,64}bit/.config but making changes to them
    should properly be done by:
         execute "./buildpbaroot" the normal way from the images directory
	 cd scratch/buildroot/32bit
	 make menuconfig
	   [Make any changes as needed]
         cd ../64bit
 	 make menuconfig
	   [Again, make any changes as needed]
         cd ..
         make O=32bit
         make O=64bit
	   [Now do whatever testing you believe is needed for the resulting
            output]
	   [Once you're happy, make sure you save those new 32bit/.config
	    and 64bit/.config files somewhere, so you can use them again
	    in the future.]
  - Linux kernel version is determined by a config line in the buildroot
    .config files above; however updating to a newer Linux kernel may
    require new kernel.config files.  Changes of that sort can be 
    accomplished similar to what's shown above:
	 execute "./buildpbaroot" the normal way from the images directory
	 cd scratch/buildroot/32bit/build/linux-4.14.146
	 make menuconfig
	   [Make any changes as needed]
         cd ../../../64bit/build/linux-4.14.146
 	 make menuconfig
	   [Again, make any changes as needed]
         cd ../../..
         make O=32bit
         make O=64bit
	   [Now do whatever testing you believe is needed for the resulting
            output kernels]
	   [Once you're happy, make sure you save those new .config files
	    for 32bit and 64bit kernels somewhere as 32bit/kernel.config
	    and 64bit/kernel.config, so you can use them again
	    in the future.]

