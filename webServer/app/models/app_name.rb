class AppName < ActiveRecord::Base
  attr_accessible :name, :abbr
  has_many   :app_lists
end
