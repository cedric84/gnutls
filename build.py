import argparse
import pathlib
import subprocess
import sys
import os

def run(*args):
	"""Runs the specified command."""
	ret = subprocess.Popen(args).wait()
	if (0 != ret):
		sys.exit(ret)

class host_t:
	"""The host class."""
	def __del__(self):
		"""The destructor."""
		pass

	def __init__(self, user_args):
		"""The main constructor."""
		#---Call parent constructor---#
		super().__init__()

		#---Zero---#
		self.install_pfx			= user_args.install_pfx.absolute()
		self.build_shared_libs		= user_args.build_shared_libs
		self.absolute_rpath			= user_args.absolute_rpath

	def configure(self, triplet, *args):
		"""Configure the build."""
		install_pfx	= self.install_pfx.joinpath(triplet)
		run("./configure",
			"--prefix=" + str(install_pfx),
			"--enable-shared" if (False != self.build_shared_libs) else "--disable-shared",
			"--enable-static" if (False == self.build_shared_libs) else "--disable-static",
			"PKG_CONFIG_LIBDIR="
				+ str(install_pfx.joinpath("lib", "pkgconfig"))
				+ ":" + str(install_pfx.joinpath("lib64", "pkgconfig"))
				,
			"GMP_LIBS=" + str(install_pfx.joinpath("lib")),
			"--with-included-libtasn1",
			"--with-included-unistring",
			"--without-p11-kit",
			"--without-idn",
			"--disable-libdane",
			"--without-tpm",
			"--disable-guile",
			"--without-clg-rpath" if (False != self.absolute_rpath) else "--with-clg-rpath",
			*args
		)

	def make(self):
		"""Runs the build."""
		run("make")

	def check(self):
		"""Checks the build."""
		run("make", "check")

	def install(self):
		"""Installs the build."""
		run("make", "install")

	def clean(self):
		"""Cleans the build."""
		run("git", "clean", "-fd")

class host_clg_pandeb9_t(host_t):
	def __del__(self):
		"""The destructor."""
		pass

	def __init__(self, user_args):
		"""The main constructor."""
		#---Call parent constructor---#
		super().__init__(user_args)

	def build_all(self):
		"""Builds all the supported targets."""
		configure_arg_arr	= [
			[
				"x86_64-linux",
				"--enable-threads=posix",
				"--with-pic",
			],
		]
		for configure_idx in range(0, len(configure_arg_arr)):
			configure_arg	= configure_arg_arr[configure_idx]
			self.configure(*configure_arg)
			self.make()
			if ((0 == configure_idx) and (False != self.absolute_rpath)):
				self.check()
			self.install()
			self.clean()






#---Define the hosts---#
host_names	= {
	"clg-pandeb9"	: host_clg_pandeb9_t,
}

#---Parse the command line---#
parser	= argparse.ArgumentParser()
parser.add_argument("host"
	, type			= str
	, choices		= host_names.keys()
	, help			= "the name of the host this script is running on."
)
parser.add_argument("--install_pfx"
	, type			= pathlib.Path
	, default		= pathlib.Path(".").resolve().joinpath("install")
	, help			= "the installation path."
)
parser.add_argument("--build_shared_libs"
	, default		= False
	, action		= "store_true"
	, help			= "says whether to build shared libraries or static ones."
)
parser.add_argument("--absolute_rpath"
	, default		= False
	, action		= "store_true"
	, help			= "says whether the RUNPATH shall be absolute or not."
)
user_args	= parser.parse_args()

#---Create the host---#
host	= host_names[user_args.host](user_args)

#---Build all---#
os.chdir(str(pathlib.Path(__file__).parent.joinpath("gnutls")))
host.build_all()
