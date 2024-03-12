cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

#add_library("AudioFile/Audiofile.h")

#add_subdirectory("AudioFile/")
#include_directories("/Users/eperets/dst5/AIHearingAid/Bernd_Repo/deepNeuronalFilter/audio_filter/audio/exp1/")

project ("AI-Hearing-Aid")
add_executable(wavRW wavRW.cpp)
