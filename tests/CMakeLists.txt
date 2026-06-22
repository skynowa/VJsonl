function(add_vjson_test target source)
    add_executable(${target}
        ${source}
    )

    target_link_libraries(${target} PRIVATE
        VJsonLib
        Qt6::Test
    )

    vjson_enable_strict_warnings(${target})

    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_vjson_test(TestJsonlModel TestJsonlModel.cpp)
add_vjson_test(TestLogFilterProxyModel TestLogFilterProxyModel.cpp)
add_vjson_test(TestUtilities TestUtilities.cpp)
add_vjson_test(TestCsv TestCsv.cpp)
add_vjson_test(TestTableSessionManager TestTableSessionManager.cpp)
