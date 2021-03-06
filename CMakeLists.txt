cmake_minimum_required(VERSION 3.5)
project(Linux_IPC C)

set(CMAKE_C_STANDARD 99)
set(GCC_COVERAGE_COMPILE_FLAGS "-g -Wall")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")

set(RTM_DIR routing_table_manager)
set(RTM_SRC ${RTM_DIR}/src)
set(UDS_DIR unix_domain_sockets)
set(UDS_NMP ${UDS_DIR}/no_multiplexing)
set(UDS_MP ${UDS_DIR}/multiplexing)
set(MQ_DIR posix_message_queues)
set(SHM_DIR posix_shared_memory)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${RTM_DIR}/include
        ${RTM_DIR}/unity
        ${RTM_SRC}
        ${UDS_MP}
)

# --------------------------------------------------------------------------------------------------
# UNIX domain sockets
# --------------------------------------------------------------------------------------------------

add_executable(client utils.c ${UDS_DIR}/client.c)

add_executable(server_no_multiplexing utils.c ${UDS_NMP}/server.c)
install(TARGETS server_no_multiplexing client DESTINATION ${UDS_NMP})

add_executable(server_multiplexing utils.c ${UDS_MP}/server.c ${UDS_MP}/fd_set_mgmt.c)
install(TARGETS server_multiplexing client DESTINATION ${UDS_DIR}/multiplexing)

# --------------------------------------------------------------------------------------------------
# POSIX message queues
# --------------------------------------------------------------------------------------------------

add_executable(mq_server utils.c ${MQ_DIR}/mq_server.c)
add_executable(mq_client utils.c ${MQ_DIR}/mq_client.c)
target_link_libraries(mq_server PUBLIC rt)
target_link_libraries(mq_client PUBLIC rt)
install(TARGETS mq_server mq_client DESTINATION ${MQ_DIR})

# --------------------------------------------------------------------------------------------------
# Shared memory
# --------------------------------------------------------------------------------------------------

add_executable(shm_reader utils.c ${SHM_DIR}/shm_reader.c)
add_executable(shm_writer utils.c  ${SHM_DIR}/shm_writer.c)
target_link_libraries(shm_reader PUBLIC rt)
target_link_libraries(shm_writer PUBLIC rt)
install(TARGETS shm_reader shm_writer DESTINATION ${SHM_DIR})

# --------------------------------------------------------------------------------------------------
# Routing table manager project
# --------------------------------------------------------------------------------------------------

find_library(GCDS gcds HINTS ${RTM_DIR}/lib)

add_executable(rtm_server utils.c ${RTM_SRC}/rtm_server.c ${RTM_SRC}/rtm.c ${RTM_SRC}/input.c
        ${UDS_MP}/fd_set_mgmt.c)
target_link_libraries(rtm_server PUBLIC ${GCDS})

add_executable(rtm_client utils.c ${RTM_SRC}/rtm_client.c ${RTM_SRC}/rtm.c ${UDS_MP}/fd_set_mgmt.c)
target_link_libraries(rtm_client PUBLIC ${GCDS})

install(TARGETS rtm_server rtm_client DESTINATION ${RTM_DIR})

set(TEST_DIR ${RTM_DIR}/tests)

add_executable(test_input ${RTM_DIR}/unity/unity.c ${TEST_DIR}/test_input.c ${RTM_SRC}/input.c
        ${RTM_SRC}/rtm.c ${RTM_DIR}/include/linked_list.h utils.c ${UDS_MP}/fd_set_mgmt.c)
target_link_libraries(test_input PUBLIC ${GCDS})
install(TARGETS test_input DESTINATION ${TEST_DIR})
