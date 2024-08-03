# Makefile for building dependencies for macOS Intel and Silicon platforms.

# Makefile options:

# Specifies number of jobs at once for one build target.
# "make NPROC=2" to build targets with only 2 processing units (threads).
# (Default: Max count of available processing units on current host)
NPROC ?= $(shell sysctl -n hw.ncpu)

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


# Apple Clang compiler deployment variables
DEPLOYMENT_TARGET_FLAGS := -mmacosx-version-min=$(MINIMUM_REQUIRED)
DEPLOYMENT_TARGET_ENV   := MACOSX_DEPLOYMENT_TARGET="$(MINIMUM_REQUIRED)"

# Need to specify "--build" argument for Ruby x86_64 and ARM64 build
ifeq ($(strip $(shell uname -m)), "arm64")
	RUBY_BUILD := aarch64-apple-darwin
else
	RUBY_BUILD := $(ARCH)-apple-darwin
endif

# Define compilers
CC  := clang -arch $(ARCH)
CXX := clang++ -arch $(ARCH)

# Declare directories
MKFDIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
PREFIX := $(MKFDIR)/build-$(ARCH)
DLDIR  := $(MKFDIR)/downloads
DLARCH := $(ARCH)
BDIR   := build-$(ARCH)
BINDIR := $(PREFIX)/bin
LIBDIR := $(PREFIX)/lib
INCDIR := $(PREFIX)/include
PKGDIR := $(PREFIX)/lib/pkgconfig

# Variables for compiling
CFLAGS  := $(DEPLOYMENT_TARGET_FLAGS) -O3 -I$(INCDIR)
LDFLAGS := -L$(LIBDIR)

# Environment variables
AC_ENV    := PKG_CONFIG_LIBDIR="$(PKGDIR)" CC="$(CC)" CXX="$(CXX)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" $(DEPLOYMENT_TARGET_ENV)
CMAKE_ENV := PKG_CONFIG_LIBDIR="$(PKGDIR)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)" $(DEPLOYMENT_TARGET_ENV)

# Autoconf options
AC_ARGS := --host="$(HOST)" --prefix="$(PREFIX)" --libdir="$(PREFIX)/lib"

# CMake options
CMAKE_ARGS := \
	-DCMAKE_INSTALL_PREFIX="$(PREFIX)" \
	-DCMAKE_PREFIX_PATH="$(PREFIX)" \
	-DCMAKE_OSX_ARCHITECTURES="$(ARCH)" \
	-DCMAKE_OSX_DEPLOYMENT_TARGET="$(MINIMUM_REQUIRED)" \
	-DCMAKE_C_FLAGS="$(CFLAGS)" \
	-DCMAKE_BUILD_TYPE=Release

# Ruby configure arguments to build
RUBY_ARGS := \
	--build="$(RUBY_BUILD)" \
	--enable-shared \
	--enable-install-static-library \
	--disable-install-doc \
	--disable-install-rdoc \
	--disable-install-capi \
	--disable-rubygems \
	--enable-mkmf-verbose \
	--without-gmp \
	--with-zlib-dir="$(PREFIX)" \
	--with-libyaml-dir="$(PREFIX)" \
	--with-libffi-dir="$(PREFIX)" \
	--with-openssl-dir="$(PREFIX)" \
	--with-static-linked-ext \
	--with-out-ext=readline,pty,syslog,win32,win32ole

# Shortcut shell build commands
GIT           := git clone -q -c advice.detachedHead=false --single-branch --no-tags --depth 1
CONFIGURE     := $(AC_ENV) ./configure $(AC_ARGS)
CMAKE         := $(CMAKE_ENV) cmake -S . -B $(BDIR) -G "Unix Makefiles" $(CMAKE_ARGS)
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
	libogg libvorbis libtheora zlib physfs uchardet libpng libjpeg \
	pixman harfbuzz freetype harfbuzz-ft sdl2 sdl2_image sdl2_ttf sdl2_sound \
	openal libyaml libffi openssl ruby ruby-ext-openssl


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

OPTS_UCHARDET := \
	-DBUILD_STATIC=ON \
	-DBUILD_SHARED_LIBS=OFF \
	-DBUILD_BINARY=OFF

OPTS_LIBPNG := \
	-DPNG_STATIC=ON \
	-DPNG_SHARED=OFF \
	-DPNG_FRAMEWORK=OFF \
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
openal: init libogg $(LIBDIR)/libopenal.a

$(LIBDIR)/libopenal.a: $(DLDIR)/openal/$(BDIR)/libopenal.a
	@printf "\e[94m=>\e[0m \e[36mInstalling OpenAL...\e[0m\n"
	@cd $(DLDIR)/openal; $(CMAKE_INSTALL)
	@touch $(LIBDIR)/libopenal.a

$(DLDIR)/openal/$(BDIR)/libopenal.a: $(DLDIR)/openal/$(BDIR)/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding OpenAL...\e[0m\n"
	@cd $(DLDIR)/openal; $(CMAKE_BUILD)

$(DLDIR)/openal/$(BDIR)/Makefile: $(DLDIR)/openal/CMakeLists.txt
	@printf "\e[94m=>\e[0m \e[36mConfiguring OpenAL...\e[0m\n"
	@cd $(DLDIR)/openal; $(CMAKE) $(OPTS_OPENAL)

$(DLDIR)/openal/CMakeLists.txt:
	@printf "\e[94m=>\e[0m \e[36mDownloading OpenAL 1.23.1...\e[0m\n"
	@$(GIT) -b 1.23.1 https://github.com/kcat/openal-soft $(DLDIR)/openal


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

$(DLDIR)/$(DLARCH)/openssl/Makefile: $(DLDIR)/$(DLARCH)/openssl/Configure
	@printf "\e[94m=>\e[0m \e[36mConfiguring OpenSSL...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/openssl; \
	CC="$(CC)" CXX="$(CXX)" CFLAGS="$(DEPLOYMENT_TARGET_FLAGS)" $(DEPLOYMENT_TARGET_ENV) \
	perl ./Configure $(OPTS_OPENSSL)

$(DLDIR)/$(DLARCH)/openssl/Configure:
	@printf "\e[94m=>\e[0m \e[36mDownloading OpenSSL 3.0.14...\e[0m\n"
	@$(GIT) -b openssl-3.0.14 https://github.com/openssl/openssl $(DLDIR)/$(DLARCH)/openssl

# ---------------------------------- Ruby 3.1 ----------------------------------
ruby: init zlib libyaml libffi openssl $(LIBDIR)/libruby.3.1.dylib

$(LIBDIR)/libruby.3.1.dylib: $(DLDIR)/$(DLARCH)/ruby/libruby.3.1.dylib
	@printf "\e[94m=>\e[0m \e[36mInstalling Ruby...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/ruby; $(AC_ENV) make install
	@install_name_tool -id @rpath/libruby.3.1.dylib $(LIBDIR)/libruby.3.1.dylib

$(DLDIR)/$(DLARCH)/ruby/libruby.3.1.dylib: $(DLDIR)/$(DLARCH)/ruby/Makefile
	@printf "\e[94m=>\e[0m \e[36mBuilding Ruby...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/ruby; $(AC_ENV) make -j $(NPROC)

$(DLDIR)/$(DLARCH)/ruby/Makefile: $(DLDIR)/$(DLARCH)/ruby/configure
	@printf "\e[94m=>\e[0m \e[36mConfiguring Ruby...\e[0m\n"
ifeq ($(LTO),1)
	@cd $(DLDIR)/$(DLARCH)/ruby; \
	export $(AC_ENV); \
	export CFLAGS="$$CFLAGS -DRUBY_FUNCTION_NAME_STRING=__func__ -flto=full"; \
	export LDFLAGS="$$LDFLAGS -flto=full"; \
	./configure $(AC_ARGS) $(RUBY_ARGS)
else
	@cd $(DLDIR)/$(DLARCH)/ruby; \
	export $(AC_ENV); \
	export CFLAGS="$$CFLAGS -DRUBY_FUNCTION_NAME_STRING=__func__"; \
	./configure $(AC_ARGS) $(RUBY_ARGS)
endif

$(DLDIR)/$(DLARCH)/ruby/configure: $(DLDIR)/$(DLARCH)/ruby/configure.ac
	@printf "\e[94m=>\e[0m \e[36mPrepare Ruby configuration files...\e[0m\n"
	@cd $(DLDIR)/$(DLARCH)/ruby; autoreconf -fi

$(DLDIR)/$(DLARCH)/ruby/configure.ac:
	@printf "\e[94m=>\e[0m \e[36mDownloading Ruby 3.1.6...\e[0m\n"
	@$(GIT) -b v3_1_6 https://github.com/ruby/ruby $(DLDIR)/$(DLARCH)/ruby
