math.randomseed(os.time())

-- load a map

Engine.load_map("outdoor.tmx")

-- load some characters
local hero = Character.new("hero.xml")
local sta = Character.new("student_a.xml", 100, 100)
local stb = Character.new("student_b.xml", 500, 100)

-- Set an hero and position it
Engine.set_hero(hero)
hero:position(400, 400)

-- Initialize engine counters, needed before the first .iteration
Engine.start()

local it = 0

repeat
    it = it + 1
    -- every ten cycles I check if the distance
    if it > 10 then
        if sta:moving() == false and sta:distance_from_obj(hero) < 50 then
            sta:goto(math.random(32, 700), math.random(32, 500))
        end
        if stb:moving() == false and stb:distance_from_obj(hero) < 50 then
            stb:goto(math.random(32, 600), math.random(32, 400))
        end
        it = 0
    end
until Engine.iteration() == false

