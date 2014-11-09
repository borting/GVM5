class VmList < ActiveRecord::Base
  attr_accessible  :cpu, :memory, :serial_no, :available, :ip
  belongs_to :user
  has_many   :app_lists, :dependent=>:destroy  
 
end
