
print("jump to lua script");

din1[0] = self:getInput(uint64_t(0));
din2[0] = self:getInput(uint64_t(1));

dout = din1[0] + din2[0];

self:setOutput(uint64_t(0), dout);

