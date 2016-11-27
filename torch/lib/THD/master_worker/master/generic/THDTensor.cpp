#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "master_worker/master/generic/THDTensor.cpp"
#else

using namespace thd;
using namespace rpc;
using namespace master;

static THDTensor* THDTensor_(_alloc)() {
  THDTensor* new_tensor = new THDTensor();
  new_tensor->tensor_id = THDState::s_nextId++;
  new_tensor->refcount = 1;
  return new_tensor;
}

THDTensor* THDTensor_(new)() {
  THDTensor* tensor = THDTensor_(_alloc)();
  std::unique_ptr<RPCMessage> construct_message = packMessage(
      Functions::construct,
      static_cast<char>(tensor_type_traits<real>::type),
      *tensor
  );
  masterCommandChannel->sendMessage(
      std::move(construct_message),
      THDState::s_current_worker
  );
  return tensor;
}

THDTensor* THDTensor_(newWithSize)(THLongStorage *sizes, THLongStorage *strides) {
  THDTensor* tensor = THDTensor_(_alloc)();
  std::unique_ptr<RPCMessage> construct_message = packMessage(
      Functions::constructWithSize,
      static_cast<char>(tensor_type_traits<real>::type),
      *tensor,
      sizes,
      strides
  );
  masterCommandChannel->sendMessage(
      std::move(construct_message),
      THDState::s_current_worker
  );
  return tensor;
}

void THDTensor_(free)(THDTensor *tensor) {
  // TODO: refcount
  std::unique_ptr<RPCMessage> free_message = packMessage(
      Functions::free,
      tensor->tensor_id
  );
  masterCommandChannel->sendMessage(
      std::move(free_message),
      THDState::s_current_worker
  );
}

#endif
