# encoding: UTF-8
require 'digest/sha1'
class User < ActiveRecord::Base
  attr_accessible :email, :name, :pw, :username, :pw_confirmation
  has_many :vm_lists
  
  attr_accessor :pw_confirmation 
  
  validates :name, :presence=>true
  validates :username, :uniqueness => true, :presence => true
  validates :email, :presence =>true
    
  validates :pw, :presence => true, :confirmation =>{:message => "Password is inconsistent"}
  validates :pw_confirmation, :presence =>true

  
#=========================================================================for pw
  def pw
    @pw
  end
 
  def pw=(pwd)
    @pw=pwd
    return if pwd.blank?
    create_new_salt
    self.hashed_pw=User.encrypted_password(self.pw,self.salt)
  end

  def self.authenticate(username,password)
    user=self.find_by_username(username)
    if user
      expected_password=encrypted_password(password,user.salt)
      if user.hashed_pw!=expected_password
        user=nil
      end
    end
    user
  end
  
 
private


  def create_new_salt
    self.salt = self.object_id.to_s+rand.to_s
  end

  def self.encrypted_password(password,salt)
    string_to_hash=password+"infor2012"+salt
    Digest::SHA1.hexdigest(string_to_hash)
  end  
  
  
end
