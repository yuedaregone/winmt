local schedule = {
	intervals = {5, 10},
	shows = {"Stop", "Continue"},
	window = nil,
	wm_t = 0,
	index = 0,
	t = 0,
	tick = false
}

function schedule:start()
	self.t = os.time()
	self.index = 0
	self.tick = true
end

function schedule:update()
	
	if not self.tick then return end
	if self.index + 1 > #self.intervals then
		self:start()
	end
	self:checkwindow()
	
	local cur = os.time()
	local gap = cur - self.t
	if gap > self.intervals[self.index + 1] then		
		self.index = self.index + 1
		self:show(self.index)
	end
end

function schedule:checkwindow()
	if self.window ~= nil and os.time() - self.wm_t > 2 then
		self:closewindow()
	end
end

function schedule:show(idx)
	self:closewindow()
	self.window = MW.new(self.shows[idx], 500, 500, 50, 50);
	self.window:show(10)
	self.wm_t = os.time()
end

function schedule:closewindow()
	if self.window ~= nil then
		self.window:stop()
		self.window = nil
	end
end

function schedule:stop()
	self.t = 0
	self.tick = false
	self:closewindow()
end

return schedule