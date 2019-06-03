# Welcome to GNUWorld

### STOP 
This repo is not official. It contains changes that are not found in the upstream repo. **USE THIS AT YOUR OWN RISK**

---------

The setup procedures listed here are generic, and do not provide any information for building databases for these two modules.

GNUWorld is an IRC services server.  This server hosts several virtual client modules that perform IRC network and channel functions. GNUWorld's official source can be located at https://github.com/UndernetIRC/gnuworld

**To use GNUWorld, you need a few of the basics:** 
* An uplink IRC server.  Make sure the U lines are set properly for the bots you wish for GNUWorld to host.
* A machine to run GNUWorld.  Keep in mind that GNUWorld uses as much bandwidth as any other IRC server on your network.
* GNUWorld will also use quite a bit of RAM time, though not normally nearly as much as other IRC servers on your network.

### Getting Started
The first step you need to take is to obtain a copy of GNUWorld. GNUWorld is software distributed under the GPL. A copy of the license is included with this distribution.

You can obtain GNUWorld through the above github repo. Issue the following command to clone the repo from Github:
```
git clone https://github.com/UndernetIRC/gnuworld
```

This will create a directory under your current directory named **gnuworld** in which all of GNUWorld's files will be placed.

### Building GNUWorld
**These instructions were tailored for a typical Ubuntu install.** They may not work on other linux distributions or on a BSD install. You will need to determine how to install the packages required to run this software on your machine. 

On ubuntu, you can install all necessary packages using this command: 
```
sudo apt-get install gcc g++ cpp flex byacc bison make automake openssl libssl-dev git libpqxx-4.0 libreadline6-dev zlib1g-dev vim
```
In order to build GNUWorld, you will need a compiler which supports the ANSI C++ Standard.  GNUmake is also recommended 
(gmake).

Change into the gnuworld directory, and run the configuration script:
```
cd gnuworld
./configure
```
When you run configure, you may specify which clients to build.  Use the ```--enable-modules``` option to specify a comma separated list of modules to build (each client is now a dynamically loadable module). Here is an example of how to configure for only the cservice client:

```
./configure --enable-modules=cservice
```

This example will build both the cservice and ccontrol (oper services) modules:

```
./configure --enable-modules=cservice,ccontrol
```

Not specifying the ```--enable-modules``` option at all will build all available modules. If you should choose to build no modules at all (which isn't very useful), you can give the "no" (without the quotes) argument to the ```--enable-modules``` option:

```
./configure --enable-modules=no
```

Note also that all of the usual configure options are supported, and may be viewed by:

```
./configure --help
```

IMPORTANT: If your OS supports shared libraries, you can reduce the compile time by half if you give configure these addional
options: ```--enable-shared --disable-static```

This will build the Makefiles that GNUWorld will use to compile its source code.  Compile gnuworld with the command:

```
make
```

This command will build all of the libraries and binaries. To install the binaries and library modules into ./bin/, type:

```
make install
```

### Configuring GNUWorld

Change into the bin directory:

```
cd bin
```

In here you will find several .conf files.  These are the files that GNUWorld uses to configure itself and its bots.  The file "GNUWorld.example.conf" contains sufficient information for GNUWorld to connect to an IRC uplink server located on the local (127.0.0.1) machine.

In order to run any services clients, each of those modules must be built and installed into the bin/ directory (see above).  To have a client loaded, add a line like this to the .conf of your choice:

```
module = libcservice.la cservice.conf
```

The "module" keyword tells gnuworld to attempt to load the given module.
**NOTE:** Individual modules may require additional configuration. Such is the case with modules libcservice (channel service) and 
ccontrol (oper services).  Please read the README files in each of those modules' subdirectories for more information.

All modules will be built with a ".la" extension.  This is a libtool archive.  LibTool is used to build dynamically loadable modules on many architectures, including those which do not support shared objects.  Therefore, your system may require static archives to be built. In either case, libtool will figure out the proper build commands, and create a ".la" archive for each client which is built.

The format of the configuration files should be fairly straightforward. GNUWorld also supports several command line options obtained by running the command:

```
gnuworld -h
```

### Running GNUWorld

In order to run GNUWorld, all you need to pass it is the name of the configuration file.  To have GNUWorld run and connect to the IRC server located at localhost:6667, using the configuration file GNUWorld.example.conf, issue the following command:

```
gnuworld -f GNUWorld.example.conf
```

That's it!  You will now have GNUWorld running!

Should you have any problems, please email us at gnuworld-devel@lists.sourceforge.net.  Don't forget to include a print out of the compiler errors, if any, or the logs from the problem.  It helps to run GNUWorld in verbose mode using the -c 
command line option. Please also include OS and compiler type and version.

You may also visit https://github.com/UndernetIRC/gnuworld/issues to submit an online bug report.

Thanks,
The GNUWorld Development Team
