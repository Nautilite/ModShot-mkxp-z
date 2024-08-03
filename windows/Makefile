# Makefile for building dependencies for Windows (MSYS2/MinGW) platforms.

# Makefile options:

# Specifies number of jobs at once for one build target.
# "make NPROC=2" to build targets with only 2 processing units (threads).
# (Default: Max count of available processing units on current host)
NPROC ?= $(shell nproc)

# Enables Link-Time Optimization for Ruby to improve performance,
# but increases compile time.
# "make LTO=1" to build Ruby with LTO.
# (Default: 0 (False))
LTO ?= 0

# Whether build SDL_image with JPEG XL (JXL) decoding support.
# This also means additional downloading libjxl and their dependencies.
# "make SDL_IMAGE_JXL=1" to build SDL_image with JPEG XL decoding support.
# (Default: 1 (True))
SDL_IMAGE_JXL ?= 1


# ==============================================================================


# Get current MSYSTEM target environment in lowercase
MSYS := $(shell echo $(MSYSTEM) | tr '[:upper:]' '[:lower:]')

# OpenSSL configure has own build targets for MSYS2/MinGW
ifneq (,$(findstring x86_64,$(MSYSTEM_CHOST)))
	OPENSSL_TARGET := mingw64
else ifneq (,$(findstring aarch64,$(MSYSTEM_CHOST)))
	# Custom target for building on CLANGARM64 environment
	# See also "openssl-mingwarm64.patch" 
	OPENSSL_TARGET := mingwarm64
else
	OPENSSL_TARGET := mingw
endif

# Ruby native libraries architecture name
RUBY_ARCH = $(shell $(BINDIR)/ruby -e "require 'rbconfig'; puts RbConfig::CONFIG['arch']")

# Get compilers and Ruby library prefix depending on MSYSTEM environment
ifeq ($(MSYS),mingw64)
	CC := gcc
	CXX := g++
	RUBY_PREFIX := x64-msvcrt-
else ifeq ($(MSYS),mingw32)
	CC := gcc
	CXX := g++
	RUBY_PREFIX := msvcrt-
else ifeq ($(MSYS),ucrt64)
	CC := gcc
	CXX := g++
	RUBY_PREFIX := x64-ucrt-
else ifeq ($(MSYS),clang64)
	CC := clang
	CXX := clang++
	RUBY_PREFIX := x64-ucrt-
else ifeq ($(MSYS),clang32)
	CC := clang
	CXX := clang++
	RUBY_PREFIX := ucrt-
else ifeq ($(MSYS),clangarm64)
	CC := clang
	CXX := clang++
	RUBY_PREFIX := x64-ucrt-
endif

# Declare directories
MKFDIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
PREFIX := $(MKFDIR)/build-$(MSYS)
DLDIR  := $(MKFDIR)/downloads
DLARCH := $(MSYS)
BDIR   := build-$(MSYS)
BINDIR := $(PREFIX)/bin
LIBDIR := $(PREFIX)/lib
INCDIR := $(PREFIX)/include
PKGDIR := $(PREFIX)/lib/pkgconfig

# Variables for compiling
CFLAGS  := -O3 -I$(INCDIR)
LDFLAGS := -L$(LIBDIR)

# Environment variables
AC_ENV    := PKG_CONFIG_LIBDIR="$(PKGDIR)" CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"
CMAKE_ENV := PKG_CONFIG_LIBDIR="$(PKGDIR)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

# Autoconf options
AC_ARGS := --prefix="$(PREFIX)" --libdir="$(PREFIX)/lib"

# CMake options
CMAKE_ARGS := \
	-DCMAKE_INSTALL_PREFIX="$(PREFIX)" \
	-DCMAKE_PREFIX_PATH="$(PREFIX)" \
	-DCMAKE_C_COMPILER="$(CC)" \
	-DCMAKE_C_FLAGS="$(CFLAGS)" \
	-DCMAKE_BUILD_TYPE=Release

# Ruby configure arguments to build
RUBY_ARGS := \
	--prefix="$(shell cygpath -m $(PREFIX))" \
	--libdir="$(shell cygpath -m $(PREFIX))/lib" \
	--enable-shared \
	--enable-install-static-library \
	--disable-install-doc \
	--disable-install-rdoc \
	--disable-install-capi \
	--disable-rubygems \
	--enable-mkmf-verbose \
	--without-gmp \
	--with-zlib-dir="$(shell cygpath -m $(PREFIX))" \
	--with-libyaml-dir="$(shell cygpath -m $(PREFIX))" \
	--with-libffi-dir="$(shell cygpath -m $(PREFIX))" \
	--with-openssl-dir="$(shell cygpath -m $(PREFIX))" \
	--with-static-linked-ext \
	--with-out-ext=openssl,readline,pty,syslog,win32,win32ole

# Shortcut shell build commands
GIT           := git clone -q -c advice.detachedHead=false --single-branch --no-tags --depth 1
CONFIGURE     := $(AC_ENV) ./configure $(AC_ARGS)
CMAKE         := $(CMAKE_ENV) cmake -S . -B $(BDIR) -G "MSYS Makefiles" $(CMAKE_ARGS)
CMAKE_BUILD   := cmake --build $(BDIR) -- -j $(NPROC)
CMAKE_INSTALL := cmake --install $(BDIR)


# ==============================================================================


all: download build


# Download target recipes
download: \
	$(DLDIR) \
	$(DLDIR)/libogg/CMakeLists.txt \
	$(DLDIR)/libvorbis/CMakeLists.txt \
	$(DLDIR)/$(DLARCH)/libtheora/autogen.sh \
	$(DLDIR)/zlib-ng/CMakeLists.txt \
	$(DLDIR)/physfs/CMakeLists.txt \
	$(DLDIR)/$(DLARCH)/libiconv/configure.ac \
	$(DLDIR)/uchardet/CMakeLists.txt \
	$(DLDIR)/libpng/CMakeLists.txt \
	$(DLDIR)/libjpeg/CMakeLists.txt \
	$(DLDIR)/$(DLARCH)/pixman/autogen.sh \
	$(DLDIR)/harfbuzz/CMakeLists.txt \
	$(DLDIR)/freetype/CMakeLists.txt \
	$(DLDIR)/sdl2/CMakeLists.txt \
	$(DLDIR)/sdl2_image/CMakeLists.txt \
	$(DLDIR)/sdl2_ttf/CMakeLists.txt \
	$(DLDIR)/sdl2_sound/CMakeLists.txt \
	$(DLDIR)/openal/CMakeLists.txt \
	$(DLDIR)/fluidsynth/CMakeLists.txt \
	$(DLDIR)/libyaml/CMakeLists.txt \
	$(DLDIR)/$(DLARCH)/libffi/configure.ac \
	$(DLDIR)/$(DLARCH)/openssl/Configure \
	$(DLDIR)/$(DLARCH)/ruby/configure.ac

# Build target recipes
build: \
	libogg \
	libvorbis \
	libtheora \
	zlib \
	physfs \
	libiconv \
	uchardet \
	libpng \
	libjpeg \
	pixman \
	harfbuzz \
	freetype \
	harfbuzz-ft \
	sdl2 \
	sdl2_image \
	sdl2_ttf \
	sdl2_sound \
	openal \
	fluidsynth \
	openssl \
	ruby \
	ruby-ext-openssl


init: $(DLDIR) $(BINDIR) $(LIBDIR) $(INCDIR)

$(DLDIR):
	@mkdir -p $(DLDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)

$(LIBDIR):
	@mkdir -p $(LIBDIR)

$(INCDIR):
	@mkdir -p $(INCDIR)


clean: clean-download clean-prefix

clean-download:
	@printf "\e[91m=>\e[0m \e[35mCleaning download folder...\e[0m\n"
	@rm -rf $(DLDIR)

clean-prefix:
	@printf "\e[91m=>\e[0m \e[35mCleaning prefix folder...\e[0m\n"
	@rm -rf $(PREFIX)


.PHONY: \
	all download build init clean clean-download clean-prefix \
	libogg libvorbis libtheora zlib physfs uchardet libiconv libpng libjpeg \
	pixman harfbuzz freetype harfbuzz-ft sdl2 sdl2_image sdl2_ttf sdl2_sound \
	openal fluidsynth libyaml libffi openssl ruby ruby-ext-openssl


# ============================ Dependencies options ============================


OPTS_LIBOGG := \
	-DBUILD_SHARED_LIBS=OFF \
	-DBUILD_TESTING=OFF \
	-DINSTALL_DOCS=OFF

OPTS_LIBVORBIS := \
	-DBUILD_SHARED_LIBS=OFF

OPTS_LIBTHEORA := \
	--disable-shared \
	--enable-static \
	--disable-doc \
	--disable-spec \
	--disable-examples \
	--disable-encode

OPTS_ZLIB := \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DBUILD_SHARED_LIBS=OFF \
	-DZLIB_COMPAT=ON \
	-DZLIB_ENABLE_TESTS=OFF \
	-DZLIBNG_ENABLE_TESTS=OFF \
	-DWITH_GTEST=OFF

OPTS_PHYSFS := \
	-DPHYSFS_BUILD_STATIC=ON \
	-DPHYSFS_BUILD_SHARED=OFF \
	-DPHYSFS_BUILD_TEST=OFF \
	-DPHYSFS_BUILD_DOCS=OFF

OPTS_LIBICONV := \
	--disable-shared \
	--enable-static

OPTS_UCHARDET := \
	-DBUILD_STATIC=ON \
	-DBUILD_SHARED_LIBS=OFF \
	-DBUILD_BINARY=OFF

OPTS_LIBPNG := \
	-DPNG_STATIC=ON \
	-DPNG_SHARED=OFF \
	-DPNG_TESTS=OFF \
	-DPNG_TOOLS=OFF

OPTS_LIBJPEG := \
	-DENABLE_STATIC=ON \
	-DENABLE_SHARED=OFF \
	-DWITH_SIMD=OFF

OPTS_PIXMAN := \
	--disable-shared \
	--enable-static \
	--disable-arm-neon \
	--disable-arm-a64-neon \
	--disable-gtk \
	--disable-libpng

OPTS_FREETYPE := \
	-DFT_REQUIRE_HARFBUZZ=ON \
	-DFT_DISABLE_BZIP2=ON \
	-DFT_DISABLE_BROTLI=ON

OPTS_SDL := \
	-DSDL_STATIC=ON \
	-DSDL_SHARED=OFF

OPTS_SDL_IMAGE := \
	-DBUILD_SHARED_LIBS=OFF \
	-DSDL2IMAGE_DEPS_SHARED=OFF \
	-DSDL2IMAGE_VENDORED=OFF \
	-DSDL2IMAGE_SAMPLES=OFF

OPTS_SDL_TTF := \
	-DBUILD_SHARED_LIBS=OFF \
	-DSDL2TTF_SAMPLES=OFF \
	-DSDL2TTF_HARFBUZZ=ON

OPTS_SDL_SOUND := \
	-DSDLSOUND_BUILD_STATIC=ON \
	-DSDLSOUND_BUILD_SHARED=OFF \
	-DSDLSOUND_BUILD_TEST=OFF

OPTS_OPENAL := \
	-DLIBTYPE=STATIC \
	-DALSOFT_UTILS=OFF \
	-DALSOFT_EXAMPLES=OFF \
	-DALSOFT_EMBED_HRTF_DATA=ON

OPTS_FLUIDSYNTH := \
	-DBUILD_SHARED_LIBS=OFF \
	-DLIB_SUFFIX="" \
	-Denable-sdl2=OFF \
	-Denable-readline=OFF

OPTS_LIBYAML := \
	-DCMAKE_POSITION_INDEPENDENT_CODE=ON \
	-DBUILD_SHARED_LIBS=OFF \
	-DBUILD_TESTING=OFF \
	-DINSTALL_CMAKE_DIR="lib/cmake/yaml"

OPTS_LIBFFI := \
	--disable-shared \
	--enable-static \
	--disable-docs

OPTS_OPENSSL := \
	--prefix="$(PREFIX)" \
	--libdir="lib" \
	--openssldir="$(PREFIX)/ssl" \
	$(OPENSSL_TARGET) \
	no-shared \
	no-makedepend \
	no-tests


# ================================= Xiph codecs ================================

# ------------------------------------- Ogg ------------------------------------
libogg: init $(LIBDIR)/libogg.a

$(LIBDIR)/libogg.a: $(DLDIR)/libogg/$(BDIR)/libogg.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libogg...\e[0m\n"
	@cd $(DLDIR)/libogg; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libogg.a

$(DLDIR)/libogg/$(BDIR)/libogg.a: $(DLDIR)/libogg/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libogg...\e[0m\n"
	@cd $(DLDIR)/libogg; $(CMAKE_BUILD)

$(DLDIR)/libogg/$(BDIR)/Makefile: $(DLDIR)/libogg/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring libogg...\e[0m\n"
	@cd $(DLDIR)/libogg; $(CMAKE) $(OPTS_LIBOGG)

$(DLDIR)/libogg/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading libogg 1.3.5...\e[0m\n"
	@$(GIT) -b v1.3.5 https://github.com/xiph/ogg $(DLDIR)/libogg

# ----------------------------------- Vorbis -----------------------------------
libvorbis: init libogg $(LIBDIR)/libvorbis.a

$(LIBDIR)/libvorbis.a: $(DLDIR)/libvorbis/$(BDIR)/lib/libvorbis.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libvorbis...\e[0m\n"
	@cd $(DLDIR)/libvorbis; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libvorbis.a

$(DLDIR)/libvorbis/$(BDIR)/lib/libvorbis.a: $(DLDIR)/libvorbis/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libvorbis...\e[0m\n"
	@cd $(DLDIR)/libvorbis; $(CMAKE_BUILD)

$(DLDIR)/libvorbis/$(BDIR)/Makefile: $(DLDIR)/libvorbis/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring libvorbis...\e[0m\n"
	@cd $(DLDIR)/libvorbis; $(CMAKE) $(OPTS_LIBVORBIS)

$(DLDIR)/libvorbis/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading libvorbis 1.3.7...\e[0m\n"
	@$(GIT) -b v1.3.7 https://github.com/xiph/vorbis $(DLDIR)/libvorbis

# ----------------------------------- Theora -----------------------------------
libtheora: init libogg libvorbis $(LIBDIR)/libtheora.a

$(LIBDIR)/libtheora.a: $(DLDIR)/$(DLARCH)/libtheora/lib/.libs/libtheora.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libtheora...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libtheora; make install

$(DLDIR)/$(DLARCH)/libtheora/lib/.libs/libtheora.a: $(DLDIR)/$(DLARCH)/libtheora/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libtheora...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libtheora; make -j $(NPROC)

$(DLDIR)/$(DLARCH)/libtheora/Makefile: $(DLDIR)/$(DLARCH)/libtheora/configure
	@printf "\e[94m=>\e[0m \e[36mConfiguring libtheora...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libtheora; $(CONFIGURE) $(OPTS_LIBTHEORA)

$(DLDIR)/$(DLARCH)/libtheora/configure: $(DLDIR)/$(DLARCH)/libtheora/autogen.sh
	@printf "\e[94m=>\e[0m \e[36mPrepare libtheora configuration files...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libtheora; autoreconf -fiv -I m4

$(DLDIR)/$(DLARCH)/libtheora/autogen.sh:
	@printf "\e[94m=>\e[0m \e[36mDownloading libtheora 1.2.0alpha1+git...\e[0m\n"
	@$(GIT) -b master https://github.com/xiph/theora $(DLDIR)/$(DLARCH)/libtheora


# =============================== Misc libraries ===============================

# ----------------------------------- Zlib-ng ----------------------------------
zlib: init $(LIBDIR)/libz.a

$(LIBDIR)/libz.a: $(DLDIR)/zlib-ng/$(BDIR)/libz.a
	@printf "\e[94m=>\e[0m \e[36mInstalling zlib-ng...\e[0m\n"
	@cd $(DLDIR)/zlib-ng; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libz.a

$(DLDIR)/zlib-ng/$(BDIR)/libz.a: $(DLDIR)/zlib-ng/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding zlib-ng...\e[0m\n"
	@cd $(DLDIR)/zlib-ng; $(CMAKE_BUILD)

$(DLDIR)/zlib-ng/$(BDIR)/Makefile: $(DLDIR)/zlib-ng/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring zlib-ng...\e[0m\n"
	@cd $(DLDIR)/zlib-ng; $(CMAKE) $(OPTS_ZLIB)

$(DLDIR)/zlib-ng/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading zlib-ng 2.2.1...\e[0m\n"
	@$(GIT) -b 2.2.1 https://github.com/zlib-ng/zlib-ng $(DLDIR)/zlib-ng

# ----------------------------------- PhysFS -----------------------------------
physfs: init $(LIBDIR)/libphysfs.a

$(LIBDIR)/libphysfs.a: $(DLDIR)/physfs/$(BDIR)/libphysfs.a
	@printf "\e[94m=>\e[0m \e[36mInstalling PhysFS...\e[0m\n"
	@cd $(DLDIR)/physfs; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libphysfs.a

$(DLDIR)/physfs/$(BDIR)/libphysfs.a: $(DLDIR)/physfs/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding PhysFS...\e[0m\n"
	@cd $(DLDIR)/physfs; $(CMAKE_BUILD)

$(DLDIR)/physfs/$(BDIR)/Makefile: $(DLDIR)/physfs/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring PhysFS...\e[0m\n"
	@cd $(DLDIR)/physfs; $(CMAKE) $(OPTS_PHYSFS)

$(DLDIR)/physfs/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading PhysFS 3.2.0...\e[0m\n"
	@$(GIT) -b release-3.2.0 https://github.com/icculus/physfs $(DLDIR)/physfs

# ---------------------------------- GNU iconv ---------------------------------
libiconv: init $(LIBDIR)/libcharset.a

$(LIBDIR)/libcharset.a: $(DLDIR)/$(DLARCH)/libiconv/lib/.libs/libiconv.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libiconv...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libiconv; make install

$(DLDIR)/$(DLARCH)/libiconv/lib/.libs/libiconv.a: $(DLDIR)/$(DLARCH)/libiconv/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libiconv...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libiconv; make -j $(NPROC)

$(DLDIR)/$(DLARCH)/libiconv/Makefile: $(DLDIR)/$(DLARCH)/libiconv/configure.ac
	@printf "\e[94m=>\e[0m \e[36mConfiguring libiconv...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libiconv; $(CONFIGURE) $(OPTS_LIBICONV)

$(DLDIR)/$(DLARCH)/libiconv/configure.ac: $(DLDIR)/libiconv-1.17.tar.gz
	@printf "\e[94m=>\e[0m \e[36mUnpacking GNU iconv...\e[0m\n"
	@tar -xzf $(DLDIR)/libiconv-1.17.tar.gz -C $(DLDIR)/$(DLARCH)/
	@mv $(DLDIR)/$(DLARCH)/libiconv-1.17 $(DLDIR)/$(DLARCH)/libiconv
	@touch $(DLDIR)/$(DLARCH)/libiconv/configure.ac

$(DLDIR)/libiconv-1.17.tar.gz:
	@printf "\e[94m=>\e[0m \e[36mDownloading GNU iconv 1.17...\e[0m\n"
	@wget -q https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.17.tar.gz -P $(DLDIR)/

# ---------------------------------- uchardet ----------------------------------
uchardet: init $(LIBDIR)/libuchardet.a

$(LIBDIR)/libuchardet.a: $(DLDIR)/uchardet/$(BDIR)/src/libuchardet.a
	@printf "\e[94m=>\e[0m \e[36mInstalling uchardet...\e[0m\n"
	@cd $(DLDIR)/uchardet; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libuchardet.a

$(DLDIR)/uchardet/$(BDIR)/src/libuchardet.a: $(DLDIR)/uchardet/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding uchardet...\e[0m\n"
	@cd $(DLDIR)/uchardet; $(CMAKE_BUILD)

$(DLDIR)/uchardet/$(BDIR)/Makefile: $(DLDIR)/uchardet/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring uchardet...\e[0m\n"
	@cd $(DLDIR)/uchardet; $(CMAKE) $(OPTS_UCHARDET)

$(DLDIR)/uchardet/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading uchardet 0.0.8...\e[0m\n"
	@$(GIT) -b v0.0.8 https://gitlab.freedesktop.org/uchardet/uchardet.git $(DLDIR)/uchardet


# =============================== Image libraries ==============================

# ----------------------------------- libpng -----------------------------------
libpng: init zlib $(LIBDIR)/libpng.a

$(LIBDIR)/libpng.a: $(DLDIR)/libpng/$(BDIR)/libpng.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libpng...\e[0m\n"
	@cd $(DLDIR)/libpng; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libpng.a

$(DLDIR)/libpng/$(BDIR)/libpng.a: $(DLDIR)/libpng/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libpng...\e[0m\n"
	@cd $(DLDIR)/libpng; $(CMAKE_BUILD)

$(DLDIR)/libpng/$(BDIR)/Makefile: $(DLDIR)/libpng/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring libpng...\e[0m\n"
	@cd $(DLDIR)/libpng; $(CMAKE) $(OPTS_LIBPNG)

$(DLDIR)/libpng/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading libpng 1.6.43...\e[0m\n"
	@$(GIT) -b v1.6.43 https://github.com/pnggroup/libpng $(DLDIR)/libpng

# ----------------------------------- libjpeg ----------------------------------
libjpeg: init $(LIBDIR)/libjpeg.a

$(LIBDIR)/libjpeg.a: $(DLDIR)/libjpeg/$(BDIR)/libjpeg.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libjpeg-turbo...\e[0m\n"
	@cd $(DLDIR)/libjpeg; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libjpeg.a

$(DLDIR)/libjpeg/$(BDIR)/libjpeg.a: $(DLDIR)/libjpeg/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libjpeg-turbo...\e[0m\n"
	@cd $(DLDIR)/libjpeg; $(CMAKE_BUILD)

$(DLDIR)/libjpeg/$(BDIR)/Makefile: $(DLDIR)/libjpeg/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring libjpeg-turbo...\e[0m\n"
	@cd $(DLDIR)/libjpeg; $(CMAKE) $(OPTS_LIBJPEG)

$(DLDIR)/libjpeg/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading libjpeg-turbo 3.0.3...\e[0m\n"
	@$(GIT) -b 3.0.3 https://github.com/libjpeg-turbo/libjpeg-turbo $(DLDIR)/libjpeg

# ----------------------------------- Pixman -----------------------------------
pixman: init libpng $(LIBDIR)/libpixman-1.a

$(LIBDIR)/libpixman-1.a: $(DLDIR)/$(DLARCH)/pixman/pixman/.libs/libpixman-1.a
	@printf "\e[94m=>\e[0m \e[36mInstalling Pixman...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/pixman; make install

$(DLDIR)/$(DLARCH)/pixman/pixman/.libs/libpixman-1.a: $(DLDIR)/$(DLARCH)/pixman/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding Pixman...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/pixman; make -j $(NPROC)

$(DLDIR)/$(DLARCH)/pixman/Makefile: $(DLDIR)/$(DLARCH)/pixman/autogen.sh
	@printf "\e[94m=>\e[0m \e[36mConfiguring Pixman...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/pixman; $(AC_ENV) ./autogen.sh $(AC_ARGS) $(OPTS_PIXMAN)

$(DLDIR)/$(DLARCH)/pixman/autogen.sh:
	@printf "\e[94m=>\e[0m \e[36mDownloading Pixman 0.42.2...\e[0m\n"
	@$(GIT) -b pixman-0.42.2 https://gitlab.freedesktop.org/pixman/pixman.git $(DLDIR)/$(DLARCH)/pixman


# ================================ Text shaping ================================

# ---------------------------------- HarfBuzz ----------------------------------
harfbuzz: init $(LIBDIR)/libharfbuzz.a

$(LIBDIR)/libharfbuzz.a: $(DLDIR)/harfbuzz/$(BDIR)/libharfbuzz.a
	@printf "\e[94m=>\e[0m \e[36mInstalling HarfBuzz...\e[0m\n"
	@cd $(DLDIR)/harfbuzz; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libharfbuzz.a

$(DLDIR)/harfbuzz/$(BDIR)/libharfbuzz.a: $(DLDIR)/harfbuzz/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding HarfBuzz...\e[0m\n"
	@cd $(DLDIR)/harfbuzz; $(CMAKE_BUILD)

$(DLDIR)/harfbuzz/$(BDIR)/Makefile: $(DLDIR)/harfbuzz/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring HarfBuzz...\e[0m\n"
	@cd $(DLDIR)/harfbuzz; $(CMAKE) -DHB_BUILD_SUBSET=NO

$(DLDIR)/harfbuzz/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading HarfBuzz 9.0.0...\e[0m\n"
	@$(GIT) -b 9.0.0 https://github.com/harfbuzz/harfbuzz $(DLDIR)/harfbuzz

# ---------------------------------- FreeType ----------------------------------
freetype: init zlib libpng harfbuzz $(LIBDIR)/libfreetype.a

$(LIBDIR)/libfreetype.a: $(DLDIR)/freetype/$(BDIR)/libfreetype.a
	@printf "\e[94m=>\e[0m \e[36mInstalling FreeType...\e[0m\n"
	@cd $(DLDIR)/freetype; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libfreetype.a

$(DLDIR)/freetype/$(BDIR)/libfreetype.a: $(DLDIR)/freetype/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding FreeType...\e[0m\n"
	@cd $(DLDIR)/freetype; $(CMAKE_BUILD)

$(DLDIR)/freetype/$(BDIR)/Makefile: $(DLDIR)/freetype/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring FreeType...\e[0m\n"
	@cd $(DLDIR)/freetype; $(CMAKE) $(OPTS_FREETYPE)

$(DLDIR)/freetype/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading FreeType 2.13.2...\e[0m\n"
	@$(GIT) -b VER-2-13-2 https://gitlab.freedesktop.org/freetype/freetype.git $(DLDIR)/freetype

# ------------------------- HarfBuzz + FreeType interop ------------------------

harfbuzz-ft: init harfbuzz freetype $(DLDIR)/harfbuzz/$(BDIR)/.ft-interop

$(DLDIR)/harfbuzz/$(BDIR)/.ft-interop:
	@printf "\e[94m=>\e[0m \e[36mBuilding HarfBuzz with FreeType interop...\e[0m\n"
	cd $(DLDIR)/harfbuzz; $(CMAKE) -DHB_HAVE_FREETYPE=ON
	cd $(DLDIR)/harfbuzz; $(CMAKE_BUILD)
	cd $(DLDIR)/harfbuzz; $(CMAKE_INSTALL)
	touch $(LIBDIR)/libharfbuzz.a
	touch $(DLDIR)/harfbuzz/$(BDIR)/.ft-interop


# ============================== SDL2 and modules ==============================

# ------------------------------------ SDL2 ------------------------------------
sdl2: init $(LIBDIR)/libSDL2.a

$(LIBDIR)/libSDL2.a: $(DLDIR)/sdl2/$(BDIR)/libSDL2.a
	@printf "\e[94m=>\e[0m \e[36mInstalling SDL2...\e[0m\n"
	@cd $(DLDIR)/sdl2; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libSDL2.a

$(DLDIR)/sdl2/$(BDIR)/libSDL2.a: $(DLDIR)/sdl2/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding SDL2...\e[0m\n"
	@cd $(DLDIR)/sdl2; $(CMAKE_BUILD)

$(DLDIR)/sdl2/$(BDIR)/Makefile: $(DLDIR)/sdl2/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring SDL2...\e[0m\n"
	@cd $(DLDIR)/sdl2; $(CMAKE) $(OPTS_SDL)

$(DLDIR)/sdl2/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading SDL2 2.30.6...\e[0m\n"
	@$(GIT) -b release-2.30.6 https://github.com/libsdl-org/SDL $(DLDIR)/sdl2

# --------------------------------- SDL2_image ---------------------------------
sdl2_image: init sdl2 libpng libjpeg $(LIBDIR)/libSDL2_image.a

$(LIBDIR)/libSDL2_image.a: $(DLDIR)/sdl2_image/$(BDIR)/libSDL2_image.a
	@printf "\e[94m=>\e[0m \e[36mInstalling SDL2_image...\e[0m\n"
	@cd $(DLDIR)/sdl2_image; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libSDL2_image.a

$(DLDIR)/sdl2_image/$(BDIR)/libSDL2_image.a: $(DLDIR)/sdl2_image/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding SDL2_image...\e[0m\n"
	@cd $(DLDIR)/sdl2_image; $(CMAKE_BUILD)

$(DLDIR)/sdl2_image/$(BDIR)/Makefile: $(DLDIR)/sdl2_image/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring SDL2_image...\e[0m\n"
	@cd $(DLDIR)/sdl2_image; $(CMAKE) $(OPTS_SDL_IMAGE)
ifeq ($(SDL_IMAGE_JXL),1)
	@cd $(DLDIR)/sdl2_image; cmake -S . -B $(BDIR) \
	-DSDL2IMAGE_VENDORED=ON -DSDL2IMAGE_JXL=ON -DJPEGXL_ENABLE_OPENEXR=OFF
endif

$(DLDIR)/sdl2_image/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading SDL2_image 2.8.2...\e[0m\n"
	@$(GIT) -b release-2.8.2 https://github.com/libsdl-org/SDL_image $(DLDIR)/sdl2_image
ifeq ($(SDL_IMAGE_JXL),1)
	@printf "\e[94m=>\e[0m \e[36mDownloading libjxl for SDL2_image...\e[0m\n"
	@cd $(DLDIR)/sdl2_image; git submodule update -q --init --recursive external/libjxl
endif

# ---------------------------------- SDL2_ttf ----------------------------------
sdl2_ttf: init sdl2 freetype harfbuzz $(LIBDIR)/libSDL2_ttf.a

$(LIBDIR)/libSDL2_ttf.a: $(DLDIR)/sdl2_ttf/$(BDIR)/libSDL2_ttf.a
	@printf "\e[94m=>\e[0m \e[36mInstalling SDL2_ttf...\e[0m\n"
	@cd $(DLDIR)/sdl2_ttf; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libSDL2_ttf.a

$(DLDIR)/sdl2_ttf/$(BDIR)/libSDL2_ttf.a: $(DLDIR)/sdl2_ttf/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding SDL2_ttf...\e[0m\n"
	@cd $(DLDIR)/sdl2_ttf; $(CMAKE_BUILD)

$(DLDIR)/sdl2_ttf/$(BDIR)/Makefile: $(DLDIR)/sdl2_ttf/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring SDL2_ttf...\e[0m\n"
	@cd $(DLDIR)/sdl2_ttf; $(CMAKE) $(OPTS_SDL_TTF)

$(DLDIR)/sdl2_ttf/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading SDL2_ttf 2.22.0...\e[0m\n"
	@$(GIT) -b release-2.22.0 https://github.com/libsdl-org/SDL_ttf $(DLDIR)/sdl2_ttf

# --------------------------------- SDL2_sound ---------------------------------
sdl2_sound: init sdl2 libogg libvorbis $(LIBDIR)/libSDL2_sound.a

$(LIBDIR)/libSDL2_sound.a: $(DLDIR)/sdl2_sound/$(BDIR)/libSDL2_sound.a
	@printf "\e[94m=>\e[0m \e[36mInstalling SDL2_sound...\e[0m\n"
	@cd $(DLDIR)/sdl2_sound; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libSDL2_sound.a

$(DLDIR)/sdl2_sound/$(BDIR)/libSDL2_sound.a: $(DLDIR)/sdl2_sound/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding SDL2_sound...\e[0m\n"
	@cd $(DLDIR)/sdl2_sound; $(CMAKE_BUILD)

$(DLDIR)/sdl2_sound/$(BDIR)/Makefile: $(DLDIR)/sdl2_sound/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring SDL2_sound...\e[0m\n"
	@cd $(DLDIR)/sdl2_sound; $(CMAKE) $(OPTS_SDL_SOUND)

$(DLDIR)/sdl2_sound/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading SDL2_sound 2.0.2...\e[0m\n"
	@$(GIT) -b v2.0.2 https://github.com/icculus/SDL_sound $(DLDIR)/sdl2_sound


# =============================== Audio backends ===============================

# ----------------------------------- OpenAL -----------------------------------
openal: init libogg $(LIBDIR)/libOpenAL32.a

$(LIBDIR)/libOpenAL32.a: $(DLDIR)/openal/$(BDIR)/libOpenAL32.a
	@printf "\e[94m=>\e[0m \e[36mInstalling OpenAL...\e[0m\n"
	@cd $(DLDIR)/openal; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libopenal.a

$(DLDIR)/openal/$(BDIR)/libOpenAL32.a: $(DLDIR)/openal/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding OpenAL...\e[0m\n"
	@cd $(DLDIR)/openal; $(CMAKE_BUILD)

$(DLDIR)/openal/$(BDIR)/Makefile: $(DLDIR)/openal/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring OpenAL...\e[0m\n"
	@cd $(DLDIR)/openal; $(CMAKE) $(OPTS_OPENAL)

$(DLDIR)/openal/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading OpenAL 1.23.1...\e[0m\n"
	@$(GIT) -b 1.23.1 https://github.com/kcat/openal-soft $(DLDIR)/openal

# ---------------------------- FluidSynth (no glib) ----------------------------
fluidsynth: init $(LIBDIR)/libfluidsynth.a

$(LIBDIR)/libfluidsynth.a: $(DLDIR)/fluidsynth/$(BDIR)/src/libfluidsynth.a
	@printf "\e[94m=>\e[0m \e[36mInstalling FluidSynth...\e[0m\n"
	@cd $(DLDIR)/fluidsynth; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libfluidsynth.a

$(DLDIR)/fluidsynth/$(BDIR)/src/libfluidsynth.a: $(DLDIR)/fluidsynth/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding FluidSynth...\e[0m\n"
	@cd $(DLDIR)/fluidsynth; $(CMAKE_BUILD)

$(DLDIR)/fluidsynth/$(BDIR)/Makefile: $(DLDIR)/fluidsynth/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring FluidSynth...\e[0m\n"
	@cd $(DLDIR)/fluidsynth; $(CMAKE) $(OPTS_FLUIDSYNTH)

$(DLDIR)/fluidsynth/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading FluidSynth 2.1.5 (no glib)...\e[0m\n"
	@$(GIT) https://github.com/mkxp-z/fluidsynth-sans-glib $(DLDIR)/fluidsynth


# ============================== Ruby 3.1 and etc. =============================

# ----------------------------- libyaml (for Ruby) -----------------------------

libyaml: init $(LIBDIR)/libyaml.a

$(LIBDIR)/libyaml.a: $(DLDIR)/libyaml/$(BDIR)/libyaml.a
	@printf "\e[94m=>\e[0m \e[36mInstalling libyaml...\e[0m\n"
	@cd $(DLDIR)/libyaml; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libyaml.a

$(DLDIR)/libyaml/$(BDIR)/libyaml.a: $(DLDIR)/libyaml/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libyaml...\e[0m\n"
	@cd $(DLDIR)/libyaml; $(CMAKE_BUILD)

$(DLDIR)/libyaml/$(BDIR)/Makefile: $(DLDIR)/libyaml/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring libyaml...\e[0m\n"
	@cd $(DLDIR)/libyaml; $(CMAKE) $(OPTS_LIBYAML)

$(DLDIR)/libyaml/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading libyaml 0.2.5...\e[0m\n"
	@$(GIT) -b 0.2.5 https://github.com/yaml/libyaml $(DLDIR)/libyaml

# --------------------------- libffi (for Fiddle ext) --------------------------
libffi: init $(LIBDIR)/libffi.a

$(LIBDIR)/libffi.a: $(DLDIR)/$(DLARCH)/libffi/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding libffi...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libffi; make -j $(NPROC) && make install

$(DLDIR)/$(DLARCH)/libffi/Makefile: $(DLDIR)/$(DLARCH)/libffi/configure
	@printf "\e[94m=>\e[0m \e[36mConfiguring libffi...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libffi; \
	export $(AC_ENV); \
	export CFLAGS="-fPIC $$CFLAGS"; \
	./configure $(AC_ARGS) $(OPTS_LIBFFI)

$(DLDIR)/$(DLARCH)/libffi/configure: $(DLDIR)/$(DLARCH)/libffi/configure.ac
	@printf "\e[94m=>\e[0m \e[36mPrepare libffi configuration files...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/libffi; autoreconf -fi

$(DLDIR)/$(DLARCH)/libffi/configure.ac:
	@printf "\e[94m=>\e[0m \e[36mDownloading libffi 3.4.6...\e[0m\n"
	@$(GIT) -b v3.4.6 https://github.com/libffi/libffi $(DLDIR)/$(DLARCH)/libffi

# --------------------------------- OpenSSL 3.0 --------------------------------
openssl: init $(LIBDIR)/libssl.a

$(LIBDIR)/libssl.a: $(DLDIR)/$(DLARCH)/openssl/libssl.a
	@printf "\e[94m=>\e[0m \e[36mInstalling OpenSSL...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/openssl; make install_sw

$(DLDIR)/$(DLARCH)/openssl/libssl.a: $(DLDIR)/$(DLARCH)/openssl/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding OpenSSL...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/openssl; make -j $(NPROC)

$(DLDIR)/$(DLARCH)/openssl/Makefile: $(DLDIR)/$(DLARCH)/openssl/Configurations/10-main.conf.orig
	@printf "\e[94m=>\e[0m \e[36mConfiguring OpenSSL...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/openssl; CC="$(CC)" CXX="$(CXX)" perl ./Configure $(OPTS_OPENSSL)

$(DLDIR)/$(DLARCH)/openssl/Configurations/10-main.conf.orig: $(DLDIR)/$(DLARCH)/openssl/Configure
	@printf "\e[94m=>\e[0m \e[36mPatching OpenSSL 10-main.conf for ARM64 support...\e[0m\n"
	patch -s -b $(DLDIR)/$(DLARCH)/openssl/Configurations/10-main.conf $(MKFDIR)/openssl-mingwarm64.patch

$(DLDIR)/$(DLARCH)/openssl/Configure:
	@printf "\e[94m=>\e[0m \e[36mDownloading OpenSSL 3.0.14...\e[0m\n"
	@$(GIT) -b openssl-3.0.14 https://github.com/openssl/openssl $(DLDIR)/$(DLARCH)/openssl

# ---------------------------------- Ruby 3.1 ----------------------------------
ruby: init zlib libyaml libffi openssl $(BINDIR)/$(RUBY_PREFIX)ruby310.dll

$(BINDIR)/$(RUBY_PREFIX)ruby310.dll: $(DLDIR)/$(DLARCH)/ruby/$(RUBY_PREFIX)ruby310.dll
	@printf "\e[94m=>\e[0m \e[36mInstalling Ruby...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/ruby; make install

$(DLDIR)/$(DLARCH)/ruby/$(RUBY_PREFIX)ruby310.dll: $(DLDIR)/$(DLARCH)/ruby/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding Ruby...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/ruby; make -j $(NPROC)

$(DLDIR)/$(DLARCH)/ruby/Makefile: $(DLDIR)/$(DLARCH)/ruby/configure
	@printf "\e[94m=>\e[0m \e[36mConfiguring Ruby...\e[0m\n"
ifeq ($(LTO),1)
	@cd $(DLDIR)/$(DLARCH)/ruby; \
	export $(AC_ENV); \
	export CFLAGS="$$CFLAGS -flto"; \
	export LDFLAGS="$$LDFLAGS -flto"; \
	./configure $(AC_ARGS) $(RUBY_ARGS)
else
	@cd $(DLDIR)/$(DLARCH)/ruby; $(CONFIGURE) $(RUBY_ARGS)
endif

$(DLDIR)/$(DLARCH)/ruby/configure: $(DLDIR)/$(DLARCH)/ruby/configure.ac
	@printf "\e[94m=>\e[0m \e[36mPrepare Ruby configuration files...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/ruby; autoreconf -fi

$(DLDIR)/$(DLARCH)/ruby/configure.ac:
	@printf "\e[94m=>\e[0m \e[36mDownloading Ruby 3.1.6...\e[0m\n"
	@$(GIT) -b v3_1_6 https://github.com/ruby/ruby $(DLDIR)/$(DLARCH)/ruby

# --------------------------- Ruby OpenSSL shared ext --------------------------
ruby-ext-openssl: openssl ruby $(LIBDIR)/ruby/3.1.0/openssl

$(LIBDIR)/ruby/3.1.0/openssl: $(DLDIR)/$(DLARCH)/ruby/ext/openssl/openssl.so
	@printf "\e[94m=>\e[0m \e[36mInstalling openssl Ruby extension...\e[0m\n"
	cd $(DLDIR)/$(DLARCH)/ruby/ext/openssl; \
	cp -pv openssl.so $(LIBDIR)/ruby/3.1.0/$(RUBY_ARCH); \
	cp -rv lib/* $(LIBDIR)/ruby/3.1.0

$(DLDIR)/$(DLARCH)/ruby/ext/openssl/openssl.so: $(DLDIR)/$(DLARCH)/ruby/ext/openssl/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding openssl Ruby extension...\e[0m\n"
	cd $(DLDIR)/$(DLARCH)/ruby/ext/openssl; make -j $(NPROC)

$(DLDIR)/$(DLARCH)/ruby/ext/openssl/Makefile:
	@printf "\e[94m=>\e[0m \e[36mConfiguring openssl Ruby extension...\e[0m\n"
	cd $(DLDIR)/$(DLARCH)/ruby/ext/openssl; $(BINDIR)/ruby extconf.rb
