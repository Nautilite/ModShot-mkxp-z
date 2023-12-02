#!/usr/bin/ruby

# Dirty script that takes all libraries in the build directories
# and dumps out fat binaries to use in Xcode project.

# Will probably have to extend it a bit to do iOS if I ever get
# that working.

if not ENV.key?("_FROM_SETUP_SH")
    STDERR.puts("You must run ./setup.sh first.")
    exit(1)
end

require 'FileUtils'

BUILD_ARM = File.join(Dir.pwd, "build-macosx-arm64", "lib")
BUILD_X86 = File.join(Dir.pwd, "build-macosx-x86_64", "lib")

DESTINATION = File.join(Dir.pwd, "build-macosx-universal", "lib")

armfiles = Dir[File.join(BUILD_ARM, "*.{a,dylib}")]
x86files = Dir[File.join(BUILD_X86, "*.{a,dylib}")]

basenames = {
    :a => armfiles.map { |f| File.basename(f) },
    :b => x86files.map { |f| File.basename(f) }
}

def lipo(a, b)
    if `lipo -info #{a}`[/is architecture: arm64/] && `lipo -info #{b}`[/is architecture: x86_64/]
        `lipo -create #{a} #{b} -o #{File.join(DESTINATION, File.basename(a))}`
    end
end

FileUtils.rm_rf(DESTINATION) if Dir.exist?(DESTINATION); FileUtils.mkdir_p(DESTINATION)

armfiles.length.times { |i|
    if basenames[:b].include?(basenames[:a][i])
        linkedfile = x86files[basenames[:b].index(basenames[:a][i])]
        lipo(armfiles[i], linkedfile)
    end
}

if Dir.exist?(BUILD_ARM)
    src_includes = BUILD_ARM.sub(/lib$/, "include")
    dst = DESTINATION.sub(/lib$/, "include")
    FileUtils.ln_s(src_includes, dst) if !Dir.exist?(dst)
end

rb = File.join(BUILD_ARM, "ruby")
FileUtils.ln_s(rb, DESTINATION) if Dir.exist?(rb)
