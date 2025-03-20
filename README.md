# rclmine

rclクライアントライブラリの実験

1. Run rclmine_sub_node

```
ros2 run rclmine rclmine_sub_node
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

## rclクライアントライブラリで実装するもの

- executor
- node

- publisher
- subscriber
- service server
- service client
- timer

- parameter service
  - Nodeがインスタンス化されるときにデフォルトで作成される

- action

etc.

### parameter
rclcppでの実装  
実体はNodeのメンバのmapで保持, インターフェースとしてNodeにservice/clientを定義  

- node_parameteres.cpp set_parameters_atomicallyでメンバ変数```std::map<std::string, ParameterInfo> parameters_;```に値を格納  
--> Nodeのメンバとしてparamを管理  
- parameter_service.cppで```get_parameters_service_```などをros serviceとして公開  
内部のnode_parameters::get_parameters()を使用してparamの実体を取得し, responseで返している  




## 参考

クライアントライブラリの実装例と使い方  
<https://github.com/ros2/rclc/tree/rolling/rclc_examples/src>

## rclpy

rclpy/rclpy/src/rclpy/client.cpp  
-->

- rclpy/rclpy/src/rclpy/_rclpy_pybind11.cpp内で実行されるPYBIND11_MODULEマクロによりC++クラスがpythonに公開される  
- define_clientの引数の型はpybind11で定義されているpy::object型  
  - 以下チュートリアルのモジュール作成にも登場  
  <https://pybind11.readthedocs.io/en/latest/classes.html>  

rclpy/rclpy/src/rclpy/client.cpp  

```cpp
void
define_client(py::object module)
{
  py::class_<Client, Destroyable, std::shared_ptr<Client>>(module, "Client")
  .def(py::init<Node &, py::object, const std::string &, py::object>())
  .def_property_readonly(
    "service_name", &Client::get_service_name,
    "Get the name of the service")
  .def_property_readonly(
    "pointer", [](const Client & client) {
      return reinterpret_cast<size_t>(client.rcl_ptr());
    },
    "Get the address of the entity as an integer")
  .def(
    "send_request", &Client::send_request,
    "Send a request")
  .def(
    "service_server_is_available", &Client::service_server_is_available,
    "Return true if the service server is available")
  .def(
    "take_response", &Client::take_response,
    "Take a received response from an earlier request")
  .def(
    "configure_introspection", &Client::configure_introspection,
    "Configure whether introspection is enabled");
}
```

- 型サポートの取得
  - py::object pymessageの中に予め"_TYPE_SUPPORT"という属性が用意されており, そこから取得  

```cpp

Client::Client(
  Node & node, py::object pysrv_type, const std::string & service_name, py::object pyqos_profile)
: node_(node)
{
  srv_type_ = static_cast<rosidl_service_type_support_t *>(common_get_type_support(pysrv_type));

  // ...
  rcl_client_options_t client_ops = rcl_client_get_default_options();

  // ...
  // Create a client
  rcl_client_ = std::shared_ptr<rcl_client_t>(
    PythonAllocator<rcl_client_t>().allocate(1),
    [node](rcl_client_t * client)
    {
      // Intentionally capture node by value so shared_ptr can be transferred to copies
      rcl_ret_t ret = rcl_client_fini(client, node.rcl_ptr());
      PythonAllocator<rcl_client_t>().deallocate(client, 1);
    });

  *rcl_client_ = rcl_get_zero_initialized_client();

  rcl_ret_t ret = rcl_client_init(
    rcl_client_.get(), node_.rcl_ptr(), srv_type_, service_name.c_str(), &client_ops);
}

```

rclpy/rclpy/src/rclpy/utils.cpp

```cpp

void *
common_get_type_support(py::object pymessage)
{
  py::object pymetaclass = pymessage.attr("__class__");

  py::object value = pymetaclass.attr("_TYPE_SUPPORT");
  auto capsule_ptr = static_cast<void *>(value.cast<py::capsule>());

  return capsule_ptr;
}
```


rclpy/rclpy/rclpy/type_support.py  

```py
class CommonMsgSrvMetaClass(ProtocolType):
    """Shared attributes between messages and services."""

    _TYPE_SUPPORT: ClassVar[Optional[PyCapsule]]

    @classmethod
    def __import_type_support__(cls) -> None:
        ...
# PyCapsule: Cのポインタを扱うためのpythonオブジェクト  
# ClassVar: クラス変数(✕インスタンス変数. クラスで共通の値)  
# Optional: NoneまたはPyCapsule型  
```

### pybind11 sample  

```cpp

class ProcessPythonObjectCpp
{
public:
  ProcessPythonObjectCpp() {}
  void processPythonObject(py::object obj)
  {
    // py::objectは以下のクラスを想定する
    // class AraiClass:
    //   def __init__(self):
    //     self.val = 10

    // self.valを取得する
    int val = obj.attr("val").cast<int>();
    std::cout << "[Cpp] val: " << val << std::endl;
  }
};
```

```py
class AraiClass:
    def __init__(self):
        self.val = 10
        print("AraiClass val:", self.val)

def main():
    arai_class = AraiClass()
    process_python_object_cpp = sample_arai.ProcessPythonObjectCpp()
    process_python_object_cpp.process_python_object(arai_class)

```
