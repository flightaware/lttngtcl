#
# For the really impatient
#
# lttng-sessiond --daemonize
# lttng create my-user-space-session
# ... in another terminal
#    tclsh8.6 test/test1.tcl
# lttng list --userspace
# lttng enable-event --userspace tcl:tcl_cmdtrace 
# lttng start
# ... in the other terminal
#     <cr>
#     <cr>
#     <cr>
# lttng stop
# lttng destroy 
# babeltrace ~/lttng-traces/my-user-space-session-*
#


#package require lttngtcl 0.1.0

load "./liblttngtcl0.2.0.so"

::lttng::cmdtrace

puts "hello"
gets stdin
puts "one"
gets stdin
puts "two"
gets stdin
puts "goodbye"

rename ::lttng::cmdtrace ""
