set (output_new  ${CMAKE_CURRENT_BINARY_DIR}/${name}_output) 

set (COM  ${CMAKE_COMMAND} -E compare_files ${output_new} ${reference})  

execute_process(
 COMMAND ${cmd}
 RESULT_VARIABLE not_successful
 OUTPUT_FILE ${output_new}
 ERROR_FILE ${output_new}.err
 TIMEOUT 600
 )

if(not_successful)
 message(SEND_ERROR "error runing test '${name}': ${err}; commande ${cmd}  ${reference} : shell output: ${not_successful}!")
endif(not_successful)

MESSAGE( "ABOUT TO compare with ${COM}")

execute_process(
 COMMAND ${COM}
 RESULT_VARIABLE not_successful
 OUTPUT_VARIABLE out
 ERROR_VARIABLE err
 TIMEOUT 600
 )

if(not_successful)
 message(SEND_ERROR "output does not match for '${name}': ${err}; ${out}; shell output: ${not_successful}!")
endif(not_successful)
endif(output)

#file(REMOVE ${output_new})

