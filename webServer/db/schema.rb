# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20130730145207) do

  create_table "app_lists", :force => true do |t|
    t.integer  "vm_list_id"
    t.integer  "port"
    t.datetime "created_at",  :null => false
    t.datetime "updated_at",  :null => false
    t.integer  "app_name_id"
  end

  create_table "app_names", :force => true do |t|
    t.string   "name"
    t.datetime "created_at", :null => false
    t.datetime "updated_at", :null => false
    t.string   "abbr"
  end

  create_table "users", :force => true do |t|
    t.string   "name"
    t.string   "username"
    t.string   "hashed_pw"
    t.string   "salt"
    t.string   "email"
    t.datetime "created_at", :null => false
    t.datetime "updated_at", :null => false
  end

  create_table "vm_lists", :force => true do |t|
    t.integer  "user_id"
    t.integer  "serial_no"
    t.string   "ip"
    t.boolean  "available"
    t.datetime "created_at", :null => false
    t.datetime "updated_at", :null => false
    t.integer  "cpu"
    t.integer  "memory"
  end

end
