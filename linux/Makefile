ARCH := $(shell uname -m)

SDL2_FLAGS       := ${SDL2_FLAGS}
SDL2_IMAGE_FLAGS := ${SDL2_IMAGE_FLAGS}
SDL2_TTF_FLAGS   := ${SDL2_TTF_FLAGS}
OPENAL_FLAGS     := -DALSOFT_CPUEXT_NEON=no ${OPENAL_FLAGS}
OPENSSL_FLAGS    := linux-$(ARCH) ${OPENSSL_FLAGS}
RUBY_FLAGS       := ${RUBY_FLAGS}

BUILD_PREFIX := ${PWD}/build-$(ARCH)
LIBDIR       := $(BUILD_PREFIX)/lib
INCDIR       := $(BUILD_PREFIX)/include
DLDIR        := ${PWD}/downloads/$(ARCH)
NPROC        := $(shell nproc)
CFLAGS       := -I$(INCDIR) -flax-vector-conversions -O3
LDFLAGS      := -L$(LIBDIR)

CC  := gcc
GIT := git clone -q --single-branch --depth 1

PKG_CONFIG_LIBDIR := $(BUILD_PREFIX)/lib/pkgconfig

CONFIGURE_ENV := \
	PKG_CONFIG_LIBDIR=$(PKG_CONFIG_LIBDIR) \
	CC="$(CC)" CFLAGS="$(CFLAGS)" LDFLAGS="$(LDFLAGS)"

CONFIGURE_ARGS := --prefix="$(BUILD_PREFIX)"

CMAKE_ARGS := \
	-DCMAKE_INSTALL_PREFIX="$(BUILD_PREFIX)" \
	-DCMAKE_PREFIX_PATH="$(BUILD_PREFIX)" \
	-DCMAKE_C_FLAGS="$(CFLAGS)" \
	-DCMAKE_BUILD_TYPE=Release

SDL2_IMAGE_CONFIGURE_ARGS := \
	--enable-png=yes \
	--enable-png-shared=no \
	--enable-jpg=yes \
	--enable-jpg-shared=no \
	--enable-avif=no \
	--enable-jxl=no \
	--enable-tif=no \
	--enable-webp=no

RUBY_CONFIGURE_ARGS := \
	--enable-install-static-library \
	--enable-shared \
	--disable-install-doc \
	--with-out-ext=openssl,readline,dbm,gdbm \
	--with-static-linked-ext \
	--disable-rubygems \
	--without-gmp

CONFIGURE := $(CONFIGURE_ENV) ./configure $(CONFIGURE_ARGS)
AUTOGEN   := $(CONFIGURE_ENV) ./autogen.sh $(CONFIGURE_ARGS)
CMAKE     := $(CONFIGURE_ENV) cmake .. $(CMAKE_ARGS)


default: everything


# ==================== Xiph audio libraries ====================

# -------------------- Ogg --------------------
libogg: init $(LIBDIR)/libogg.a

$(LIBDIR)/libogg.a: $(DLDIR)/ogg/Makefile
	cd $(DLDIR)/ogg; make -j$(NPROC); make install

$(DLDIR)/ogg/Makefile: $(DLDIR)/ogg/configure
	cd $(DLDIR)/ogg; $(CONFIGURE) --enable-static=true --enable-shared=false

$(DLDIR)/ogg/configure: $(DLDIR)/ogg/autogen.sh
	cd $(DLDIR)/ogg; ./autogen.sh

$(DLDIR)/ogg/autogen.sh:
	$(GIT) https://github.com/xiph/ogg $(DLDIR)/ogg

# -------------------- Vorbis --------------------
libvorbis: init libogg $(LIBDIR)/libvorbis.a

$(LIBDIR)/libvorbis.a: $(LIBDIR)/libogg.a $(DLDIR)/vorbis/Makefile
	cd $(DLDIR)/vorbis; make -j$(NPROC); make install

$(DLDIR)/vorbis/Makefile: $(DLDIR)/vorbis/configure
	cd $(DLDIR)/vorbis; $(CONFIGURE) --with-ogg=$(BUILD_PREFIX) \
	--enable-static=true --enable-shared=false

$(DLDIR)/vorbis/configure: $(DLDIR)/vorbis/autogen.sh
	cd $(DLDIR)/vorbis; ./autogen.sh

$(DLDIR)/vorbis/autogen.sh:
	$(GIT) https://github.com/xiph/vorbis $(DLDIR)/vorbis

# -------------------- Theora --------------------
libtheora: init libvorbis libogg $(LIBDIR)/libtheora.a

$(LIBDIR)/libtheora.a: $(LIBDIR)/libogg.a $(DLDIR)/theora/Makefile
	cd $(DLDIR)/theora; make -j$(NPROC); make install

$(DLDIR)/theora/Makefile: $(DLDIR)/theora/configure
	cd $(DLDIR)/theora; $(CONFIGURE) --with-ogg=$(BUILD_PREFIX) \
	--enable-static=true --enable-shared=false --disable-examples

$(DLDIR)/theora/configure: $(DLDIR)/theora/autogen.sh
	cd $(DLDIR)/theora; ./autogen.sh

$(DLDIR)/theora/autogen.sh:
	$(GIT) https://github.com/xiph/theora $(DLDIR)/theora


# ==================== Image libraries ====================

# -------------------- libpng --------------------
libpng: init $(LIBDIR)/libpng.a

$(LIBDIR)/libpng.a: $(DLDIR)/libpng/Makefile
	cd $(DLDIR)/libpng; make -j$(NPROC); make install

$(DLDIR)/libpng/Makefile: $(DLDIR)/libpng/configure
	cd $(DLDIR)/libpng; $(CONFIGURE) --enable-static=yes --enable-shared=no

$(DLDIR)/libpng/configure:
	$(GIT) https://github.com/glennrp/libpng $(DLDIR)/libpng

# -------------------- libjpeg --------------------
libjpeg: init $(LIBDIR)/libjpeg.a

$(LIBDIR)/libjpeg.a: $(DLDIR)/libjpeg/cmakebuild/Makefile
	cd $(DLDIR)/libjpeg/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/libjpeg/cmakebuild/Makefile: $(DLDIR)/libjpeg/CMakeLists.txt
	cd $(DLDIR)/libjpeg; mkdir -p cmakebuild; cd cmakebuild; \
	$(CMAKE) -DENABLE_STATIC=yes -DENABLE_SHARED=no

$(DLDIR)/libjpeg/CMakeLists.txt:
	$(GIT) https://github.com/libjpeg-turbo/libjpeg-turbo $(DLDIR)/libjpeg

# -------------------- Pixman --------------------
pixman: init libpng $(LIBDIR)/libpixman-1.a

$(LIBDIR)/libpixman-1.a: $(DLDIR)/pixman/Makefile
	cd $(DLDIR)/pixman; make -j$(NPROC); make install

$(DLDIR)/pixman/Makefile: $(DLDIR)/pixman/autogen.sh
	cd $(DLDIR)/pixman; $(AUTOGEN) --enable-static=yes --enable-shared=no

$(DLDIR)/pixman/autogen.sh:
	$(GIT) https://github.com/freedesktop/pixman $(DLDIR)/pixman


# ==================== Misc libraries ====================

# -------------------- PhysFS --------------------
physfs: init $(LIBDIR)/libphysfs.a

$(LIBDIR)/libphysfs.a: $(DLDIR)/physfs/cmakebuild/Makefile
	cd $(DLDIR)/physfs/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/physfs/cmakebuild/Makefile: $(DLDIR)/physfs/CMakeLists.txt
	cd $(DLDIR)/physfs; mkdir cmakebuild; cd cmakebuild; \
	$(CMAKE) -DPHYSFS_BUILD_STATIC=true -DPHYSFS_BUILD_SHARED=false

$(DLDIR)/physfs/CMakeLists.txt:
	$(GIT) https://github.com/icculus/physfs $(DLDIR)/physfs

# -------------------- uchardet --------------------
uchardet: init $(LIBDIR)/libuchardet.a

$(LIBDIR)/libuchardet.a: $(DLDIR)/uchardet/cmakebuild/Makefile
	cd $(DLDIR)/uchardet/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/uchardet/cmakebuild/Makefile: $(DLDIR)/uchardet/CMakeLists.txt
	cd $(DLDIR)/uchardet; mkdir cmakebuild; cd cmakebuild; \
	$(CMAKE) -DBUILD_SHARED_LIBS=no

$(DLDIR)/uchardet/CMakeLists.txt:
	$(GIT) https://github.com/freedesktop/uchardet $(DLDIR)/uchardet

# -------------------- Freetype2 --------------------
freetype: init $(LIBDIR)/libfreetype.a

$(LIBDIR)/libfreetype.a: $(DLDIR)/freetype/Makefile
	cd $(DLDIR)/freetype; make -j$(NPROC); make install

$(DLDIR)/freetype/Makefile: $(DLDIR)/freetype/configure
	cd $(DLDIR)/freetype; $(CONFIGURE) --enable-static=true --enable-shared=false

$(DLDIR)/freetype/configure: $(DLDIR)/freetype/autogen.sh
	cd $(DLDIR)/freetype; ./autogen.sh

$(DLDIR)/freetype/autogen.sh:
	$(GIT) https://github.com/freetype/freetype $(DLDIR)/freetype

# -------------------- GNU iconv --------------------
iconv: init $(LIBDIR)/libiconv.a

$(LIBDIR)/libiconv.a: $(DLDIR)/libiconv-1.16/Makefile
	cd $(DLDIR)/libiconv-1.16; make; make install

$(DLDIR)/libiconv-1.16/Makefile: $(DLDIR)/libiconv-1.16/configure
	cd $(DLDIR)/libiconv-1.16; $(CONFIGURE) --enable-static=true --enable-shared=false

$(DLDIR)/libiconv-1.16/configure: $(DLDIR)/libiconv-1.16.tar.gz
	cd $(DLDIR); tar -xzf libiconv-1.16.tar.gz

$(DLDIR)/libiconv-1.16.tar.gz:
	wget https://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.16.tar.gz -P $(DLDIR)


# ==================== SDL2 and addons ====================

# -------------------- SDL2 --------------------
sdl2: init $(LIBDIR)/libSDL2.a

$(LIBDIR)/libSDL2.a: $(DLDIR)/sdl2/Makefile
	cd $(DLDIR)/sdl2; make -j$(NPROC); make install

$(DLDIR)/sdl2/Makefile: $(DLDIR)/sdl2/configure
	cd $(DLDIR)/sdl2; \
	$(CONFIGURE) --enable-static=true --enable-shared=false $(SDL2_FLAGS)

$(DLDIR)/sdl2/configure: $(DLDIR)/sdl2/autogen.sh
	cd $(DLDIR)/sdl2; ./autogen.sh

$(DLDIR)/sdl2/autogen.sh:
	$(GIT) https://github.com/mkxp-z/SDL $(DLDIR)/sdl2 -b mkxp-z

# -------------------- SDL2_image --------------------
sdl2image: init sdl2 libpng libjpeg $(LIBDIR)/libSDL2_image.a

$(LIBDIR)/libSDL2_image.a: $(DLDIR)/sdl2_image/Makefile
	cd $(DLDIR)/sdl2_image; make -j$(NPROC); make install

$(DLDIR)/sdl2_image/Makefile: $(DLDIR)/sdl2_image/configure
	cd $(DLDIR)/sdl2_image; \
	LIBPNG_LIBS="-L$(LIBDIR) -lpng" LIBPNG_CFLAGS="-I$(INCDIR)" \
	$(CONFIGURE) --enable-static=true --enable-shared=false \
	$(SDL2_IMAGE_CONFIGURE_ARGS) $(SDL2_IMAGE_FLAGS)

$(DLDIR)/sdl2_image/configure: $(DLDIR)/sdl2_image/autogen.sh
	cd $(DLDIR)/sdl2_image; ./autogen.sh

$(DLDIR)/sdl2_image/autogen.sh:
	$(GIT) https://github.com/mkxp-z/SDL_image_old $(DLDIR)/sdl2_image -b mkxp-z

# -------------------- SDL2_sound --------------------
sdl2sound: init sdl2 libogg libvorbis $(LIBDIR)/libSDL2_sound.a

$(LIBDIR)/libSDL2_sound.a: $(DLDIR)/sdl2_sound/cmakebuild/Makefile
	cd $(DLDIR)/sdl2_sound/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/sdl2_sound/cmakebuild/Makefile: $(DLDIR)/sdl2_sound/CMakeLists.txt
	cd $(DLDIR)/sdl2_sound; mkdir -p cmakebuild; cd cmakebuild; \
	$(CMAKE) -DSDLSOUND_BUILD_SHARED=false -DSDLSOUND_BUILD_TEST=false

$(DLDIR)/sdl2_sound/CMakeLists.txt:
	$(GIT) https://github.com/icculus/SDL_sound $(DLDIR)/sdl2_sound

# -------------------- SDL2_ttf --------------------
sdl2ttf: init sdl2 freetype $(LIBDIR)/libSDL2_ttf.a

$(LIBDIR)/libSDL2_ttf.a: $(DLDIR)/sdl2_ttf/cmakebuild/Makefile
	cd $(DLDIR)/sdl2_ttf/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/sdl2_ttf/cmakebuild/Makefile: $(DLDIR)/sdl2_ttf/CMakeLists.txt
	cd $(DLDIR)/sdl2_ttf; mkdir cmakebuild; cd cmakebuild; \
	$(CMAKE) -DBUILD_SHARED_LIBS=no

$(DLDIR)/sdl2_ttf/CMakeLists.txt:
	$(GIT) https://github.com/mkxp-z/SDL_ttf $(DLDIR)/sdl2_ttf


# ==================== Audio backends ====================

# -------------------- OpenAL --------------------
openal: init libogg $(LIBDIR)/libopenal.a

$(LIBDIR)/libopenal.a: $(DLDIR)/openal/cmakebuild/Makefile
	cd $(DLDIR)/openal/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/openal/cmakebuild/Makefile: $(DLDIR)/openal/CMakeLists.txt
	cd $(DLDIR)/openal; mkdir cmakebuild; cd cmakebuild; \
	$(CMAKE) -DLIBTYPE=STATIC -DALSOFT_EXAMPLES=no -DALSOFT_UTILS=no \
	$(OPENAL_FLAGS)

$(DLDIR)/openal/CMakeLists.txt:
	$(GIT) https://github.com/kcat/openal-soft $(DLDIR)/openal

# -------------------- FluidSynth --------------------
fluidsynth: init $(LIBDIR)/libfluidsynth.a

$(LIBDIR)/libfluidsynth.a: $(DLDIR)/fluidsynth/cmakebuild/Makefile
	cd $(DLDIR)/fluidsynth/cmakebuild; make -j$(NPROC); make install

$(DLDIR)/fluidsynth/cmakebuild/Makefile: $(DLDIR)/fluidsynth/CMakeLists.txt
	cd $(DLDIR)/fluidsynth; mkdir cmakebuild; cd cmakebuild; \
	$(CMAKE) -DBUILD_SHARED_LIBS=no -Denable-sdl2=no -Denable-readline=no

$(DLDIR)/fluidsynth/CMakeLists.txt:
	$(GIT) https://github.com/mkxp-z/fluidsynth-sans-glib $(DLDIR)/fluidsynth


# ==================== Ruby 3.1 and OpenSSL ====================

# -------------------- OpenSSL --------------------
openssl: init $(LIBDIR)/libssl.a

$(LIBDIR)/libssl.a: $(DLDIR)/openssl/Makefile
	cd $(DLDIR)/openssl; make -j$(NPROC); make install_sw

$(DLDIR)/openssl/Makefile: $(DLDIR)/openssl/Configure
	cd $(DLDIR)/openssl; \
	./Configure $(OPENSSL_FLAGS) no-shared \
	--prefix="$(BUILD_PREFIX)" --openssldir="$(BUILD_PREFIX)"

$(DLDIR)/openssl/Configure:
	$(GIT) https://github.com/openssl/openssl $(DLDIR)/openssl -b OpenSSL_1_1_1i

# -------------------- Ruby 3.1 --------------------
ruby: init openssl $(BUILD_PREFIX)/libruby.so.3.1

$(BUILD_PREFIX)/libruby.so.3.1: $(DLDIR)/ruby/Makefile
	cd $(DLDIR)/ruby; make -j$(NPROC); make install

$(DLDIR)/ruby/Makefile: $(DLDIR)/ruby/configure
	cd $(DLDIR)/ruby; $(CONFIGURE) $(RUBY_CONFIGURE_ARGS) $(RUBY_FLAGS)

$(DLDIR)/ruby/configure: $(DLDIR)/ruby/*.c
	cd $(DLDIR)/ruby; autoreconf -i

$(DLDIR)/ruby/*.c:
	$(GIT) https://github.com/mkxp-z/ruby $(DLDIR)/ruby -b mkxp-z-3.1


init:
	@mkdir -p $(LIBDIR) $(INCDIR)

powerwash: clean-downloads

clean-downloads:
	-rm -rf downloads

deps-core: libvorbis libtheora libpng libjpeg pixman physfs uchardet iconv sdl2 sdl2image sdl2sound sdl2ttf openal fluidsynth openssl
everything: deps-core ruby
