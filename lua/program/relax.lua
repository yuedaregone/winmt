local relax = {
	interval = 1800,
	tick = false,
	tm = 0,
	layer = nil,
	sprite = nil,
	cmp = nil
}
function relax:start()
	self.tick = true
	self.tm = os.time()	
	self:showlayer()
	if self.cmp ~= nil then
		self.sprite:remove(self.cmp)
		self.sprite:setrotation(0)
	end
end

function relax:showlayer()
	if self.layer == nil then
		self.layer = ML.new("relax", 1920, 1080-90, 48, 48)
		self.sprite = Sprite.new("x_happy.bin")
		self.layer:add(self.sprite)
	end
end

function relax:showtip()
	self.tick = false
	if self.cmp == nil then
		self.cmp = COMP.new()	
		local r = 0
		self.cmp:reg(function()
			r = r + 1
			if r > 360 then
				r = 0
			end
			self.sprite:setrotation(r)
		end)			
	end	
	self.sprite:add(self.cmp)
end

function relax:setkeylisten()
	keyevent.events[1] = function(k,c)
		print(1, k, c)
		if k == 1 and c == 32 and keyevent.clicked(160) then
			relax:start()
		end
		return false
	end
end

function relax:update()
	if not self.tick then return end

	local cur = os.time()
	if cur - self.tm > self.interval then
		self:showtip()
		return
	end	
end

function relax:stop()
end

return relax