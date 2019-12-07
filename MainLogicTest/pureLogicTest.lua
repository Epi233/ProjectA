
print("jump to lua script");

din1 = self:getInput(uint64_t(0));
din2 = self:getInput(uint64_t(1));

print("get input");
print(din1);
print(din2);

print("");
dout = { din1[0] + din2[0] };

self:setOutput(uint64_t(0), dout);

