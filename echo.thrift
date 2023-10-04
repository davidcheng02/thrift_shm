struct Message {
    1: string msg,
}

service HelloSvc {
  string hello(1:Message msg);
}
