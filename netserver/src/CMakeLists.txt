aux_source_directory(server SERVER)
aux_source_directory(client CLIENT)

add_executable(server ${SERVER})
add_executable(client ${CLIENT})