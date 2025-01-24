# rclmine
rclクライアントライブラリの実験用


1. Run rclmine_sub_node
```
$ ros2 run rclmine rclmine_sub_node
```
```
[Main] start main
[Main] Done rcl_get_zero_initialized_init_options()
[Main] Done rcl_get_zero_initialized_context()
[Main] Done rcl_init_options_init()
[Main] Done rcl_init()
[MyNode::Constructor] MyNode constructor
[MyNode::Constructor] MyNode handle created
[MyNode::Constructor] Context and MyNode Options are initialized
[MyNode::Constructor] MyNode is constructed
[MySubscription::Constructor] MySubscription constructor
[MySubscription::Constructor] Subscription is constructed
[Main] Subscription created
[MyNode::subscribe] Timeout
[MyNode::subscribe] Timeout
[MyNode::subscribe] Timeout
...
```

2. Run rclmine_pub_node
```
ros2 run rclmine rclmine_pub_node
```
```
[Main] start main
[Main] Done rcl_get_zero_initialized_init_options()
[Main] Done rcl_get_zero_initialized_context()
[Main] Done rcl_init_options_init()
[Main] Done rcl_init()
[MyNode::Constructor] MyNode constructor
[MyNode::Constructor] MyNode handle created
[MyNode::Constructor] Context and MyNode Options are initialized
[MyNode::Constructor] MyNode is constructed
[MyPublisher::Constructor] MyPublisher constructor
[MyPublisher::Constructor] Publisher is constructed
[Main] Publisher created
[MyPublisher::publish] Publishing: Hello, world! 0
[MyPublisher::publish] Publishing: Hello, world! 1
[MyPublisher::publish] Publishing: Hello, world! 2
[MyPublisher::publish] Publishing: Hello, world! 3
...
```

The output on the subscriber side will be as follows.  
```
[MyNode::subscribe] Timeout
[MyNode::subscribe] Timeout
[MyNode::subscribe] Timeout
[MyNode::subscribe] Received: Hello, world! 0
[MyNode::subscribe] Timeout
[MyNode::subscribe] Received: Hello, world! 1
[MyNode::subscribe] Timeout
[MyNode::subscribe] Received: Hello, world! 2
[MyNode::subscribe] Timeout
[MyNode::subscribe] Received: Hello, world! 3
...
```
